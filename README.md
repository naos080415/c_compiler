# c_compiler

## なんかおかしいところ
まぁ,そこまでやばいエラーではないと思うので,いっとき無視したいなぁ.gccもよくわからんしなぁ

### forループ内での変数初期化による無限ループ
自作コンパイラ
```
for(i=0;i<5;i++){
  i = 0;
}
```
上記のコードを実行すると無限ループになるはずだが,ならない・・・.セグフォが出る.
gccコンパイラ

```
int num = 100;
char *name;
sprintf(name,"%d",num);
for(i=0;i<5;i++){
  i = 0;
}
```
上記のコードを実行すると無限ループになる.しかし,forの前にsprintf関数をおいて実行するとセグフォが出る.これこそ謎.
clangはちゃんとsprintfがあるとき??なんか他にも要素があるかも?(for内で変数定義,printf("%s",name)での読み出し)も無限ループになるよ????.ワケワカメ.

gccのsprintf関数には問題がありそう
→ポインタで文字列を受けるとエラーになる.配列で受けると大丈夫



git push できないとき
上記
エラー表示
```
nao@naoMBP 9cc % git push -u origin master
To https://github.com/naos080415/c_compiler.git
 ! [rejected]        master -> master (non-fast-forward)
error: failed to push some refs to 'https://github.com/naos080415/c_compiler.git'
hint: Updates were rejected because the tip of your current branch is behind
hint: its remote counterpart. Integrate the remote changes (e.g.
hint: 'git pull ...') before pushing again.
hint: See the 'Note about fast-forwards' in 'git push --help' for details.
```

原因
リモート追跡ブランチにremotes/origin/masterが残っているため

ブランチ名に/が使われている場合,スラッシュ内で同名の単語があるとpushできない.

git branch -a でブランチの確認ができる.

```
nao@naoMBP 9cc % git branch -a
* master
  remotes/origin/master
```

対策
git hubは一人でのみ利用していたの

参考サイト:https://qiita.com/chiaki-kjwr/items/118a5b3237c78d720582
