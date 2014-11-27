# Developing Memo

## 命名規則について
cでは何が一般的？

## Files

### source.txt
Hello, world!
出力用

### tmp.bf
インタプリタ未完成時に適宜実行確認用

## 制御文字の出力について
どう扱うべきか？

### DEMO
bug1.cをコンパイルし、bug1.bfを取って実行すると文字化けするようになる

参考: <http://e-words.jp/p/r-ascii.html>
9:tab
10:改行
27:esc
32:space
33~126:文字

## feature
codeRun()内、文字を入力部分で複数文字入力されたときの対処はしていない
→最初の一文字を取り、あとは捨てる

