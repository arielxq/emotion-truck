const fs = require('fs');
const path = require('path');

const glbPath = path.join(
  __dirname,
  'models/02_Electronics.glb'
);

const buffer = fs.readFileSync(glbPath);

// GLB Header
const magic = buffer.toString('utf8', 0, 4);
const version = buffer.readUInt32LE(4);
const totalLength = buffer.readUInt32LE(8);

if (magic !== 'glTF') {
  throw new Error('不是有效的 GLB 檔案');
}

// 第一個 Chunk
const chunkLength = buffer.readUInt32LE(12);
const chunkType = buffer.readUInt32LE(16);

if (chunkType !== 0x4E4F534A) {
  throw new Error('找不到 GLB JSON Chunk');
}

const jsonText = buffer
  .toString('utf8', 20, 20 + chunkLength);

const gltf = JSON.parse(jsonText);

const expectedRoots = [
  'Wheel_F_root',
  'Wheel_B_root',
  'Cabins_root',
  'Signal_root',
  'LED_root',
  'Speaker_root',
  'Buzzer_root',
  'Body_base_root',
  'Body_Cover_root',
  'LCD_root',
  'ESP32_root',
  'Joystick_root',
  'OpenBin_root'
];

const nodeNames = new Set(
  (gltf.nodes || []).map(node => node.name)
);

console.log('========== GLB ROOT CHECK ==========');
console.log('Magic:', magic);
console.log('Version:', version);
console.log('GLB size:', totalLength);
console.log('');

let passed = 0;

for (const name of expectedRoots) {
  if (nodeNames.has(name)) {
    console.log('[OK]', name);
    passed++;
  } else {
    console.log('[MISSING]', name);
  }
}

console.log('');
console.log(`Result: ${passed}/${expectedRoots.length} roots found`);
