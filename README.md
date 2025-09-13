# SIC Assembler 專案
這是我的 SIC Assembler 專案，展示簡報投影片圖片。

## 1️檔案與資料結構
opcode.txt：存放操作碼（OPCODE）對應的十六進位機器碼。
source.txt：要組譯的 SIC 程式。
intermediate.txt：Pass1 生成的中間檔，包含每行指令的地址。
objectcode.txt：Pass2 生成的每行指令的機器碼對應表。
objectprogram.txt：Pass2 生成的完整目標程式（可直接載入記憶體執行）。

## 主要資料結構
二叉搜尋樹 (Node, root)
用來儲存符號表（Symbol Table），每個 label 對應它的記憶體地址。
可快速查找符號地址。
哈希表 MyHashMap
用來儲存操作碼（opcode → machine code）。
查找速度快，避免每次都遍歷檔案。

## 函式簡介
intToHex(int num, int width)
將整數轉成固定寬度的十六進位字串，用於顯示地址或物件碼。
insertNode / search / printTree
插入符號到二叉樹。
根據符號名稱查找地址。
列印整個符號表（按照字母順序）。
createHashtable()
讀取 opcode.txt，把每個操作碼存入哈希表。

## Pass 1 – 建立符號表 & 計算地址
讀取 source.txt 的每一行指令。
處理 START 設定起始地址。
遇到 label 就加入符號表（二叉樹）。
計算每條指令的地址（LOCCTR, location counter）。
處理指令類型：
WORD → +3
RESW → 3 × operand
RESB → operand
BYTE → 依內容計算長度
其他操作碼 → +3
輸出 intermediate.txt，每行包含地址、label、opcode、operand。
最後印出符號表。

## Pass 2 – 生成目標程式
讀取 intermediate.txt。
根據操作碼和符號表生成對應的機器碼：
OPCODE + 符號地址
特殊處理 RSUB、WORD、BYTE。
建立 Text Record（每 30 bytes 為一段）。
輸出：
objectcode.txt → 每行指令對應的地址與機器碼
objectprogram.txt → 標準 SIC 目標程式格式
H (Header)
T (Text)
E (End)

## main() 流程
建立操作碼哈希表 (createHashtable())
列印哈希表所有桶內容 (hashMap.printAllBuckets())
執行 Pass 1 (pass1())
執行 Pass 2 (pass2())

##簡單概括
Pass1 → 建符號表 + 計算地址 + 輸出中間檔
Pass2 → 用符號表 + 操作碼表生成目標程式
輸入 → source.txt, opcode.txt
輸出 → intermediate.txt, objectcode.txt, objectprogram.txt
