# FibermeshCurve2TressFX
ZBrushのFibermeshから"Export Curve"した.objをAMD TressFXの.tfxへ変換するプログラム

1. ZBrushのFibermeshで髪束を作る。４千ポリくらいだと上位GPUでギリギリ動くくらい？

2. スケールを100くらいに設定

![](http://i.imgur.com/L1uy0Fd.jpg)

3. Fibermeshで作った髪束をSubtoolパネルで選択。Fibermeshパネルから"Export Curve"

![](http://i.imgur.com/Lq5ri4B.jpg)

4. 頭部（髪以外）をSubtoolパネルで選択。objエクスポート。

![](http://i.imgur.com/JvJApX6.jpg)

5. 適当なソフトで頭部objを開いてマテリアルを作って適応。TressFXデモは１つのマテリアルしかサポートしていないので注意。

6. 3でエクスポートした髪束objを当ソフトへドロップ。.tfxに変換される

![](http://i.imgur.com/0RKzNRN.jpg)

7. 5のobj、6のtfx、２ファイルを適当にバイナリ形式(objb, tfxb)に変換。詳しくはTressFXのSDKを見てちょ

8. 最後に適当なファイルにリネームしてテスト。ruby.tfxbが初期表示されるファイル。

関連リンク :

[AMD TressFX](http://developer.amd.com/tools-and-sdks/graphics-development/amd-radeon-sdk/ "")
