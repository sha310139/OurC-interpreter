# OurC-interpreter
一個約5000行的類C語言直譯器(可執行迴圈和functions)<br>
此專案為中原大學「程式語言」必修課之project 占分70%<br>
共分成4個部分<br>
Project 1為 基礎運算式檢查、求值及變數宣告<br>
Project 2為 簡單C語言的文法檢查<br>
Project 3為 Project 2的進階，需要確認文法的正確性並且求出值<br>
Porject 4為 Project 3的進階，加上Call function並執行的功能<br>
每個project會有10-16題的測試，每一題約有3-4個測試數據(包含可見數據及隱藏數據)<br>
本人完成4個projects，通過所有測試，於此作業拿到滿分，且本班僅兩位滿分。<br>
此為個人作品。<br>
<br>
<br>

## 開發平台
Windows10<br>
<br>
<br>

## 開發環境
Dev C++<br>
<br>
<br>

## 檔案說明
* Our-C-Grammar-2016-07-15.doc : 文法及支援token的清單
* PL_Priject4.cpp : 主程式碼
<br>
<br>

## 功能
* 根據輸入的字串，判斷是否為正確的C語言文法並求出運算值
* 當輸入錯誤時會印出該錯誤之行數及錯誤類型(lexical error、syntactical error (token recognized)、semantic error (grammar ok))
* 支援語法包含if、else、while、cout、define function等等
* 支援陣列宣告及陣列值的運算
* 支援全域及區域變數的宣告
* 提供 ListVariable("變數名稱")、ListAllVariables()、 ListFunction("名稱")、ListAllFunctions() 等4個功能函數
* 可以根據輸入的字串 判斷是否為正確的C語言文法及輸出運算
* 可define functions(包含遞迴、function裡再call別的function等等)及執行functions
* function參數傳遞支援call by reference & call by value
* 輸入 Done(); 可使程式結束執行
<br>
<br>

## 使用方法
第一行須輸入數字(限整數)作為test number用<br>
enter後將顯示互動式介面資訊<br>
接著可輸入C語言之簡單程式碼<br>
此介面會直譯並檢查語法是否有錯<br>
接著執行該程式碼之動作，並顯示相關結果<br>
目前支援之語法包含if、else、while、cout、define function等等<br>
<br>
<br>

## I/O範例
* 簡單function defined及執行<br>
<img src="https://github.com/sha310139/OurC-interpreter/blob/main/Pictures/2.JPG" alt="Sample"  width="500">
<br>
<br>
-----------------------------------------------------------------<br>
* 陣列及迴圈運算<br>
<img src="https://github.com/sha310139/OurC-interpreter/blob/main/Pictures/3.JPG" alt="Sample"  width="300">
<br>
<br>
-----------------------------------------------------------------<br>
* function defined、執行、error顯示及功能函數運算<br>
<img src="https://github.com/sha310139/OurC-interpreter/blob/main/Pictures/5.JPG" alt="Sample"  width="500">
<br>
<br>
