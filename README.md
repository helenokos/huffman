#用 c++ 實做 Hufman 壓縮,解壓縮檔案的簡易小工具  
  
使用方法：  
(1) 壓縮 : ./huffman.exe -c -i infile -o compressfile.txt  
(2) 解壓 : ./huffman.exe -u -i compressfile.txt -o uncompressfile

舉例：  
./huffman.exe -c -i cat.jpeg -o compress.txt  
./huffman.exe -u -i compress.txt -o copy.jpeg
