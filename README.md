# FibermeshCurve2TressFX
ZBrushのFibermeshから"Export Curve"した.objをAMD TressFXの.tfxへ変換するプログラム

1. ZBrushのFibermeshで髪束を作る。４千ポリくらいだと上位GPUでギリギリ動くくらい？

2. ![](http://i.imgur.com/L1uy0Fd.jpg) スケールを100くらいに設定

3. ![](http://i.imgur.com/Lq5ri4B.jpg) Fibermeshで作った髪束をSubtoolパネルで選択。Fibermeshパネルから"Export Curve"

4. ![](http://i.imgur.com/JvJApX6.jpg) 頭部（髪以外）をSubtoolパネルで選択。objエクスポート。

5. 適当なソフトで頭部objを開いてマテリアルを作って適応。TressFXデモは１つのマテリアルしかサポートしていないので注意。

6. ![](http://i.imgur.com/0RKzNRN.jpg) 3でエクスポートした髪束objを当ソフトへドロップ。.tfxに変換される

7. 5,6のobj２つを適当にバイナリ形式(objb, tfxb)に変換。詳しくはTressFXのSDKを見てちょ

8. 最後に適当なファイルにリネームしてテスト。ruby.tfxbが初期表示されるファイル。

[AMD TressFX](http://developer.amd.com/tools-and-sdks/graphics-development/amd-radeon-sdk/ "")」
