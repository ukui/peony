echo "update translations..."
lupdate $(dirname $0)/../libpeony-qt/libpeony-qt.pro
lupdate $(dirname $0)/../src/src.pro
lupdate $(dirname $0)/../peony-qt-desktop/peony-qt-desktop.pro

echo "generate new translate resources files"
for file in $(dirname $0)/*/*.ts
do
	ts="*.ts"
	qm="*.qm"
	target=${file%.*}.qm
	echo ${target}
	echo "release $file $target"
	lrelease $file $target
done
echo "done"
