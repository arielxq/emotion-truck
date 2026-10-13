from pathlib import Path

# 你提供的專案樹狀結構文字
tree_text = """
emotion-truck-backend/
├── app/
│   ├── __init__.py
│   ├── main.py
│   ├── config.py
│   ├── db.py
│   ├── models.py
│   ├── security.py
│   ├── services/
│   │   ├── __init__.py
│   │   ├── state_service.py
│   │   ├── schedule_service.py
│   │   └── stats_service.py
│   └── routers/
│       ├── __init__.py
│       ├── device.py
│       ├── sim.py
│       ├── log.py
│       ├── stats.py
│       └── ws.py
├── static/
│   ├── index.html
│   ├── sim.html
│   ├── history.html
│   ├── stats.html
│   ├── css/
│   │   └── style.css
│   └── js/
│       ├── api.js
│       ├── sim.js
│       ├── index.js
│       └── stats.js
├── sql/
│   ├── schema.sql
│   └── seed.sql
├── requirements.txt
└── README.md
"""

def generate_from_tree_text(text: str, base_dir: str = "."):
    lines = text.strip().splitlines()
    # 用來記錄每一層路徑的堆疊
    path_stack = [Path(base_dir)]
    
    for line in lines:
        if not line.strip():
            continue
            
        # 計算縮排深度：透過計算前面包含多少個字元（如 │, └──, ├── 等與空格）來判斷層級
        # 簡單的方法是計算有幾個階層符號
        indent_level = 0
        cleaned_line = line
        
        # 移除繪圖符號，計算縮排
        for char in ["│   ", "    ", "├── ", "└── "]:
            while cleaned_line.startswith(char):
                cleaned_line = cleaned_line[len(char):]
                indent_level += 1
                
        name = cleaned_line.strip()
        if not name:
            continue
            
        # 調整堆疊到對應的深度
        # indent_level + 1 因為根目錄算一層
        while len(path_stack) > indent_level + 1:
            path_stack.pop()
            
        current_parent = path_stack[-1]
        target_path = current_parent / name.rstrip("/")
        
        # 判斷是資料夾還是檔案（有結尾斜線或是沒有副檔名的通常是資料夾，但這裡我們根據有沒有點或結尾斜線判斷）
        # 為了保險：如果名字裡面沒有副檔名（或者結尾是 /），我們就當作資料夾
        is_dir = name.endswith("/") or ("." not in name)
        
        if is_dir:
            target_path.mkdir(parents=True, exist_ok=True)
            path_stack.append(target_path)
            print(f"已建立資料夾: {target_path}")
        else:
            # 確保父資料夾存在
            target_path.parent.mkdir(parents=True, exist_ok=True)
            # 建立空檔案
            target_path.touch(exist_ok=True)
            print(f"已建立檔案: {target_path}")

if __name__ == "__main__":
    print("開始根據樹狀圖建立專案結構...")
    generate_from_tree_text(tree_text)
    print("完成！")