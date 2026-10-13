import random

PROMPTS = [
    {"text": "嗷，小心哦", "result": "COLLECTED", "weight": 50},
    {"text": "嗷，謝謝啊", "result": "COLLECTED", "weight": 25},
    {"text": "唉，這是回收哦", "result": "RETURNED", "weight": 25},
]


def pick_worker_prompt():
    return random.choices(PROMPTS, weights=[p["weight"] for p in PROMPTS], k=1)[0]
