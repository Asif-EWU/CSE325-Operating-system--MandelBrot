
all: mandel mandelmovie run

mandel: mandel.o bitmap.o
	gcc mandel.o bitmap.o -o mandel -lpthread

mandel.o: mandel.c
	gcc -Wall -g -c mandel.c -o mandel.o

bitmap.o: bitmap.c
	gcc -Wall -g -c bitmap.c -o bitmap.o

mandelmovie: mandelmovie.c
	gcc mandelmovie.c -o mandelmovie -lm

run: mandelmovie
	./mandelmovie -n 10 -p 5

clean:
	rm -f mandel.o bitmap.o mandel mandelmovie mandel.mpg m1.bmp m2.bmp m3.bmp m4.bmp m5.bmp m6.bmp m7.bmp m8.bmp m9.bmp m10.bmp m11.bmp m12.bmp m13.bmp m14.bmp m15.bmp m16.bmp m17.bmp m18.bmp m19.bmp m20.bmp m21.bmp m22.bmp m23.bmp m24.bmp m25.bmp m26.bmp m27.bmp m28.bmp m29.bmp m30.bmp m31.bmp m32.bmp m33.bmp m34.bmp m35.bmp m36.bmp m37.bmp m38.bmp m39.bmp m40.bmp m41.bmp m42.bmp m43.bmp m44.bmp m45.bmp m46.bmp m47.bmp m48.bmp m49.bmp m50.bmp

clean_image:
	rm -f mandel.mpg m1.bmp m2.bmp m3.bmp m4.bmp m5.bmp m6.bmp m7.bmp m8.bmp m9.bmp m10.bmp m11.bmp m12.bmp m13.bmp m14.bmp m15.bmp m16.bmp m17.bmp m18.bmp m19.bmp m20.bmp m21.bmp m22.bmp m23.bmp m24.bmp m25.bmp m26.bmp m27.bmp m28.bmp m29.bmp m30.bmp m31.bmp m32.bmp m33.bmp m34.bmp m35.bmp m36.bmp m37.bmp m38.bmp m39.bmp m40.bmp m41.bmp m42.bmp m43.bmp m44.bmp m45.bmp m46.bmp m47.bmp m48.bmp m49.bmp m50.bmp
