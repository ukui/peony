#!/bin/bash
shopt -s globstar nullglob extglob
for i in src/**/*.@(cpp|h);
do
  if ! grep -q Copyright $i
  then
    cat COPYING $i >$i.new && mv $i.new $i
  fi
done

for i in peony-qt-desktop/**/*.@(cpp|h)
do
	if ! grep -q Copyright $i
	then
		cat COPYING $i >$i.new && mv $i.new $i
	fi
done

for i in test/**/*.@(cpp|h);
do
	if ! grep -q Copyright $i
	then
		cat COPYING $i >$i.new && mv $i.new $i
	fi
done

for i in plugin/**/*.@(cpp|h);
do
        if ! grep -q Copyright $i
        then
                cat COPYING.EXTENSIONS $i >$i.new && mv $i.new $i
        fi
done

for i in **/*.@(cpp|h);
do
        if ! grep -q Copyright $i
        then
                cat COPYING.LIB $i >$i.new && mv $i.new $i
        fi
done
