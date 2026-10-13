// ===== 狀態機 =====
const State = {
    REST_CLOSED: 'REST_CLOSED',
    IDLE: 'IDLE',
    ACCUMULATING: 'ACCUMULATING',
    FULL: 'FULL',
    APPROACHING: 'APPROACHING',
    ARRIVED: 'ARRIVED',
    THROW_READY: 'THROW_READY',
    THROWING: 'THROWING',
    RESPONDING_CAREFUL: 'RESPONDING_CAREFUL',
    RESPONDING_THANKS: 'RESPONDING_THANKS',
    RESPONDING_RECYCLE: 'RESPONDING_RECYCLE',
    COLLECTED: 'COLLECTED',
    REST_DAILY_LIMIT: 'REST_DAILY_LIMIT'
};

const Emotion = { NONE: 'NONE', GAN: 'GAN', KAO: 'KAO', CAO: 'CAO' };

const EmotionText = {
    GAN: 'ㄍㄢˋ · 憤怒',
    KAO: 'ㄎㄠˋ · 不爽',
    CAO: 'ㄘㄠˋ · 煩躁'
};

const Emoji = {
    REST: '😴',
    IDLE: '🙂',
    ACCUMULATING: '😠',
    FULL: '😤',
    APPROACHING: '😲',
    ARRIVED: '😊',
    THROW_READY: '🗑',
    THROWING: '🗑',
    CAREFUL: '😉',
    THANKS: '😊',
    RECYCLE: '😠',
    COLLECTED: '😌',
    DAILY_LIMIT: '😴'
};

// ===== 狀態變數 =====
let currentState = State.REST_CLOSED;
let bags = 0;
let dailyCount = 0;
let selectedEmotion = Emotion.NONE;
let leftCount = 0, rightCount = 0, upCount = 0;

// 旋轉偵測
let rotationDirection = 'NONE';
let lastSector = 'CENTER';
let rotationSteps = 0;
let completedRounds = 0;

// 超時計時器
let throwReadyTimeout = null;

// ===== 狀態進入 =====
function enterState(newState) {
    currentState = newState;
    log(`State: ${newState}`);
    
    switch (newState) {
        case State.REST_CLOSED:
            setLcd('Truck resting', Emoji.REST);
            setLed('off');
            setBuzzer('無聲');
            break;
            
        case State.IDLE:
            setLcd(`Bags: ${bags}/5`, Emoji.IDLE);
            setLed('off');
            setBuzzer('無聲');
            break;
            
        case State.ACCUMULATING:
            setLcd(`${selectedEmotion} 1/3`, Emoji.ACCUMULATING);
            setLed('blink-100');
            setBuzzer('累積音效');
            playSound('accumulating');
            break;
            
        case State.FULL:
            setLcd('Emotion is full', Emoji.FULL);
            setLed('blink-200');
            setBuzzer('FULL 音效');
            playSound('full');
            break;
            
        case State.APPROACHING:
            setLcd('Truck coming', Emoji.APPROACHING);
            setLed('blink-300');
            setBuzzer('Fur Elise 漸強');
            playSound('approaching');
            break;
            
        case State.ARRIVED:
            setLcd('Truck is here', Emoji.ARRIVED);
            setLed('on');
            setBuzzer('無聲');
            resetRotation();
            break;
            
        case State.THROW_READY:
            setLcd('Press to throw', Emoji.THROW_READY);
            setLed('blink-100x2');
            setBuzzer('THROW 提示音');
            playSound('throw');
            startThrowReadyTimeout();
            break;
            
        case State.THROWING:
            setLcd('Throwing...', Emoji.THROWING);
            setLed('blink-100x2');
            setBuzzer('THROW 音效');
            break;
            
        case State.RESPONDING_CAREFUL:
            setLcd('Be careful', Emoji.CAREFUL);
            setLed('blink-100x2');
            setBuzzer('CAREFUL 音效');
            playSound('careful');
            break;
            
        case State.RESPONDING_THANKS:
            setLcd('Thank you', Emoji.THANKS);
            setLed('blink-100x2');
            setBuzzer('THANKS 音效');
            playSound('thanks');
            break;
            
        case State.RESPONDING_RECYCLE:
            setLcd(`Remain: ${bags} bags`, Emoji.RECYCLE);
            setLed('blink-200x5');
            setBuzzer('RECYCLE 音效');
            playSound('recycle');
            break;
            
        case State.COLLECTED:
            setLcd('Emotion cleared', Emoji.COLLECTED);
            setLed('on');
            setBuzzer('Fur Elise 漸弱');
            break;
            
        case State.REST_DAILY_LIMIT:
            setLcd('2 trips done', Emoji.DAILY_LIMIT);
            setLed('off');
            setBuzzer('無聲');
            break;
    }
    
    updateStatusUI();
    reportState();
}

// ===== 搖桿處理 =====function handleJoystick(direction) {
    if (currentState === State.IDLE || currentState === State.ACCUMULATING) {
        handleEmotion(direction);
    } else if (currentState === State.ARRIVED) {
        handleRotation(direction);
    }
}

function handleEmotion(direction) {
    switch (direction) {
        case 'LEFT':
            leftCount++; rightCount = 0; upCount = 0;
            log(`LEFT ${leftCount}/3`);
            if (leftCount >= 3) {
                selectedEmotion = Emotion.GAN;
                leftCount = 0;
                enterState(State.ACCUMULATING);
            } else {
                setLcd(`LEFT ${leftCount}/3`, Emoji.ACCUMULATING);
            }
            break;
        case 'RIGHT':
            rightCount++; leftCount = 0; upCount = 0;
            log(`RIGHT ${rightCount}/3`);
            if (rightCount >= 3) {
                selectedEmotion = Emotion.KAO;
                rightCount = 0;
                enterState(State.ACCUMULATING);
            } else {
                setLcd(`RIGHT ${rightCount}/3`, Emoji.ACCUMULATING);
            }
            break;
        case 'UP':
            upCount++; leftCount = 0; rightCount = 0;
            log(`UP ${upCount}/3`);
            if (upCount >= 3) {
                selectedEmotion = Emotion.CAO;
                upCount = 0;
                enterState(State.ACCUMULATING);
            } else {
                setLcd(`UP ${upCount}/3`, Emoji.ACCUMULATING);
            }
            break;
        case 'DOWN':
            leftCount = rightCount = upCount = 0;
            selectedEmotion = Emotion.NONE;
            log('Cancelled');
            enterState(State.IDLE);
            break;
    }
}

// ===== 旋轉偵測 =====
function handleRotation(direction) {
    if (direction === 'CENTER') {
        lastSector = 'CENTER';
        return;
    }
    
    if (lastSector === 'CENTER') {
        lastSector = direction;
        return;
    }
    
    // 判斷是否相鄰
    const sectors = ['UP', 'RIGHT', 'DOWN', 'LEFT'];
    const lastIdx = sectors.indexOf(lastSector);
    const currIdx = sectors.indexOf(direction);
    
    if (currIdx === -1) return;
    
    const diff = (currIdx - lastIdx + 4) % 4;
    
    if (diff === 1) {
        // 順時針
        if (rotationDirection === 'NONE') {
            rotationDirection = 'CW';
            log('Rotation direction: CLOCKWISE');
        }
        if (rotationDirection === 'CW') {
            rotationSteps++;
        }
    } else if (diff === 3) {
        // 逆時針
        if (rotationDirection === 'NONE') {
            rotationDirection = 'CCW';
            log('Rotation direction: COUNTERCLOCKWISE');
        }
        if (rotationDirection === 'CCW') {
            rotationSteps++;
        }
    } else {
        // 反向或跳躍，重新計算
        log('Direction reversed, resetting rotation');
        resetRotation();
        lastSector = direction;
        return;
    }
    
    lastSector = direction;
    
    // 每 4 步為一圈
    if (rotationSteps >= 4) {
        rotationSteps = 0;
        completedRounds++;
        log(`Completed round ${completedRounds}/3`);
        
        if (completedRounds >= 3) {
            enterState(State.THROW_READY);
        }
    }
}

function resetRotation() {
    rotationDirection = 'NONE';
    lastSector = 'CENTER';
    rotationSteps = 0;
    completedRounds = 0;
}

// ===== SW 處理 =====
function handleButton() {
    if (currentState === State.ACCUMULATING && selectedEmotion !== Emotion.NONE) {
        bags++;
        log(`Confirmed ${selectedEmotion}, bags=${bags}/5`);
        reportEmotion(selectedEmotion);
        selectedEmotion = Emotion.NONE;
        leftCount = rightCount = upCount = 0;
        
        if (bags >= 5) {
            enterState(State.FULL);
        } else {
            enterState(State.IDLE);
        }
    } else if (currentState === State.THROW_READY) {
        clearTimeout(throwReadyTimeout);
        enterState(State.THROWING);
        setTimeout(() => triggerResponding(), 1000);
    }
}

// ===== 工作人員回應 =====
function triggerResponding() {
    const r = Math.floor(Math.random() * 100);
    let prompt, newBags, returned = 0;
    
    if (r < 50) {
        prompt = 'CAREFUL';
        newBags = Math.max(0, bags - 5);
        log(`Worker: CAREFUL, bags ${bags} → ${newBags}`);
        bags = newBags;
        enterState(State.RESPONDING_CAREFUL);
    } else if (r < 75) {
        prompt = 'THANKS';
        newBags = Math.max(0, bags - 5);
        log(`Worker: THANKS, bags ${bags} → ${newBags}`);
        bags = newBags;
        enterState(State.RESPONDING_THANKS);
    } else {
        prompt = 'RECYCLE';
        returned = Math.min(Math.floor(Math.random() * 4) + 1, bags);
        newBags = bags - returned;
        log(`Worker: RECYCLE, returned ${returned}, bags ${bags} → ${newBags}`);
        bags = newBags;
        enterState(State.RESPONDING_RECYCLE);
    }
    
    reportCollection(prompt, newBags + returned, newBags, returned);
    
    setTimeout(() => {
        enterState(State.COLLECTED);
        dailyCount++;
        log(`Daily count: ${dailyCount}/2`);
        
        setTimeout(() => {
            if (dailyCount >= 2) {
                bags = 0;
                enterState(State.REST_DAILY_LIMIT);
            } else {
                enterState(State.IDLE);
            }
        }, 2000);
    }, 2000);
}

// ===== THROW_READY 超時 =====
function startThrowReadyTimeout() {
    throwReadyTimeout = setTimeout(() => {
        log('Throw timeout, returning to ARRIVED');
        setLcd('Please throw again', Emoji.ARRIVED);
        resetRotation();
        enterState(State.ARRIVED);
    }, 10000);
}

// ===== UI 更新 =====
function setLcd(line1, emoji) {
    document.getElementById('lcd-line1').textContent = line1;
    document.getElementById('lcd-emoji').textContent = emoji;
}

function setLed(mode) {
    const circle = document.getElementById('led-circle');
    const status = document.getElementById('led-status');
    
    circle.className = 'led-circle';
    status.textContent = mode;
    
    if (mode === 'on') {
        circle.classList.add('on');
    } else if (mode.startsWith('blink')) {
        const parts = mode.split('-');
        const interval = parseInt(parts[1]);
        const times = parts[2] ? parseInt(parts[2].replace('x', '')) : 0;
        blinkLed(interval, times);
    }
}

let ledTimer = null;
function blinkLed(interval, times) {
    if (ledTimer) clearInterval(ledTimer);
    const circle = document.getElementById('led-circle');
    let count = 0;
    let on = false;
    
    ledTimer = setInterval(() => {
        on = !on;
        circle.classList.toggle('on', on);
        if (on) count++;
        if (times > 0 && count >= times) {
            clearInterval(ledTimer);
            circle.classList.remove('on');
        }
    }, interval);
}

function setBuzzer(text) {
    document.getElementById('buzzer-status').textContent = text;
    document.getElementById('buzzer-icon').textContent = 
        text === '無聲' ? '🔇' : '🔊';
}

function updateStatusUI() {
    document.getElementById('current-state').textContent = currentState;
    document.getElementById('bags-count').textContent = `${bags}/5`;
    document.getElementById('daily-count').textContent = `${dailyCount}/2`;
}

// ===== 音效 =====
function playSound(type) {
    // 使用 Web Audio API 模擬
    // 實際可載入音檔
    log(`Playing sound: ${type}`);
}

// ===== 上報後端 =====
async function reportState() {
    try {
        await apiPost('/api/sim/state', {
            device_id: 'SIM-001',
            state: currentState,
            bags: bags,
            daily_count: dailyCount
        });
    } catch (e) {
        log(`Report error: ${e.message}`, true);
    }
}

async function reportEmotion(emotion) {
    try {
        await apiPost('/api/sim/emotion', {
            device_id: 'SIM-001',
            direction: 'LEFT',
            emotion_type: emotion,
            emotion_text: EmotionText[emotion] || ''
        });
    } catch (e) {
        log(`Emotion report error: ${e.message}`, true);
    }
}

async function reportCollection(prompt, before, after, returned) {
    try {
        await apiPost('/api/sim/collection', {
            device_id: 'SIM-001',
            worker_prompt: prompt,
            bags_before: before,
            bags_after: after,
            returned_bags: returned
        });
    } catch (e) {
        log(`Collection report error: ${e.message}`, true);
    }
}

// ===== 事件綁定 =====
document.addEventListener('DOMContentLoaded', () => {
    // 搖桿按鈕
    document.querySelectorAll('.dir-btn').forEach(btn => {
        btn.addEventListener('click', () => {
            handleJoystick(btn.dataset.dir);
        });
    });
    
    // SW 按鈕
    document.getElementById('sw-btn').addEventListener('click', handleButton);
    
    // 測試按鈕
    document.querySelectorAll('.test-buttons button').forEach(btn => {
        btn.addEventListener('click', () => {
            const test = btn.dataset.test;
            if (test === 'reset') {
                bags = 0;
                dailyCount = 0;
                selectedEmotion = Emotion.NONE;
                leftCount = rightCount = upCount = 0;
                resetRotation();
                enterState(State.IDLE);
            } else if (test.startsWith('state ')) {
                const stateName = test.replace('state ', '').toUpperCase();
                const stateMap = {
                    'REST': State.REST_CLOSED,
                    'IDLE': State.IDLE,
                    'APPROACHING': State.APPROACHING,
                    'ARRIVED': State.ARRIVED,
                    'THROW_READY': State.THROW_READY,
                    'THROWING': State.THROWING,
                    'CAREFUL': State.RESPONDING_CAREFUL,
                    'THANKS': State.RESPONDING_THANKS,
                    'RECYCLE': State.RESPONDING_RECYCLE,
                    'COLLECTED': State.COLLECTED,
                    'DAILY_LIMIT': State.REST_DAILY_LIMIT
                };
                if (stateMap[stateName]) {
                    enterState(stateMap[stateName]);
                }
            }
        });
    });
    
    // 初始狀態
    enterState(State.IDLE);
});