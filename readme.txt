ピーガーマガジン1978年12月号のコラム「肛門期」に掲載されていた伝説のSPC700 for MeSX(MSX)の回路

任天堂SFC(前期ロット)に内蔵されていたSPC700音源モジュールをMeSX(MSX)に接続する為のインターフェース回路です。

インターフェース仕様

MeSX(MSX)          SPC700
IOPORT(RW) 0x18    $2140(ADDR0)
IOPORT(RW) 0x19    $2141(ADDR1)
IOPORT(RW) 0x1a    $2142(ADDR2)
IOPORT(RW) 0x1b    $2143(ADDR3)
IOPORT(R)  0x1c    リセット(0x1cを読むとSPC700にリセットがかかります)
IOPORT(R)  0x1d, 0x1e, 0x1fは0x1cのミラー

回路図の他、デモ・サンプルとしてWindowsPCからMeSX(MSX)へspcファイルを転送、再生する為のコマンドツールとROMも用意しました。別途、ここにあるspc700.uf2ファームウェアを書込みしたMSXπTypeAが必要です。

構成図
[WindwosPC(playspc)] <= VCCを切断したUSBケーブル => [MSXπ] [MeSX(MSX)] [SPC700] => [音声アンプ] => ♫♫♫

* spc700.uf2

i8251機能と下記のspc700.romの複合ファームウェアです。(MSXπTypeA用)

* playspc

spcファイルをMeSX(MSX)に転送、再生する為のWindowsコマンドツールです。
別途、snesapu.dllが必要です。playspc.exeと同じディレクトリに置いてください。
ソースをコンパイルする場合は32ビットターゲットのgcc(mingw)を使用して下さい。

snesapu.dll:
https://github.com/dgrfactory/spcplay/releases (Improved SNESAPU.DLL onlyがそれです)

使用法
playspc <MSXπのシリアルポート名> <spcファイル>

例:
playspc COM1 song.spc

* spc700.rom

playspcから受け取ったデータをSPC700に受け渡す為のMeSX(MSX)用ROM(0x4000-0x7fff)プログラムです。spc700.uf2ファームウェアに内包されていますのでspc700.uf2を使用する場合は不要です。
素のi8251ファームウェアを使用する場合は何らかの方法でこのROMを起動して下さい。
ソースをコンパイルする場合はsdccを使用して下さい。
