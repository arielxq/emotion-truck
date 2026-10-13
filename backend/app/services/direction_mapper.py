EMOTIONS = {
    "UP": {"text": "ㄍㄢˋ", "name": "憤怒", "color": "RED"},
    "LEFT": {"text": "ㄎㄠˋ", "name": "不爽", "color": "ORANGE"},
    "RIGHT": {"text": "ㄘㄠˋ", "name": "煩躁", "color": "YELLOW"},
}


def emotion_for(direction: str):
    return EMOTIONS.get(direction)
