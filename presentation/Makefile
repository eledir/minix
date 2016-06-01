.PHONY: all

# all: presentation.pdf
# 	convert -density 300 presentation.pdf -resize 1024x768 -posterize 4 presentation.png

all:
	rm tmp*.png ../minix/drivers/storage/ramdisk/presentation-*.png || true
	latex presentation.tex
	dvipng -D 250 presentation.dvi -o presentation-%d.png
	for i in $$(ls -1 presentation*.png | sort -V); do echo $$i; convert $$i -separate -swap 0,2 -combine -resize 640x480 -alpha activate ../minix/drivers/storage/ramdisk/$$i; done
