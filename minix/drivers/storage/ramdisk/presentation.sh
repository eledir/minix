IMG_CUR=0
IMG_MIN=1
IMG_MAX=13

while true
do
	if [ "$(cat /gpio/SLIDE_PREV)" = "0" ] && [ $IMG_CUR -gt $IMG_MIN ]
	then
		IMG_CUR=$((IMG_CUR - 1))
		readpng /etc/presentation-$IMG_CUR.png > /dev/fb0
	fi
	if [ "$(cat /gpio/SLIDE_NEXT)" = "0" ] && [ $IMG_CUR -lt $IMG_MAX ]
	then
		IMG_CUR=$((IMG_CUR + 1))
		readpng /etc/presentation-$IMG_CUR.png > /dev/fb0
	fi
done
