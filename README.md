# 用 c++ 實做 Hufman 壓縮,解壓縮檔案的簡易小工具  
  
## 使用方法：  
(1) 壓縮 : ./huffman.exe -c -i infile -o compressfile.txt  
(2) 解壓 : ./huffman.exe -u -i compressfile.txt -o uncompressfile

舉例：  
./huffman.exe -c -i cat.jpeg -o compress.txt  
./huffman.exe -u -i compress.txt -o copy.jpeg

## 作法：
(1) 將編碼表儲存於檔案開頭  
(2) Huffman tree 的左子樹為 0 ，右子樹為 1  
(3) 合併兩個 node 時，字典序較小的為左子樹，反之為右子樹  
(4) 若超過兩個 node 出現相同頻率時，則先合併字典順序上最小的兩個 node  
