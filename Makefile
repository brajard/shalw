GENOPT= -E
YAOPT= +g -x shalw
#YAOBIN=${YAODIR}/etc/YaoI
#YAOBIN=${YAODIR}/etc/yao92
YAOBIN=./yao92

OPT = direct
#OPT = preprocessing

ifeq ($(OPT),preprocessing)
	dep = shalw_tmp.d
else
	dep = shalw.d
endif

shalw:  $(dep) shalw.h renorm.h
ifeq ($(OPT),preprocessing)
	echo "option with preprocessing"
	rm -f Y2shalw_tmp.h
	rm -f Y2shalw_tmp.h
	rm -f Yshalw_tmp.cpp
	${YAOBIN} ${YAOPT} shalw_tmp.d
	ln -s Yworkdir/Y2shalw_tmp.h
	ln -s Yworkdir/Yshalw_tmp.cpp
else
	echo "direct compilation"
	${YAOBIN} ${YAOPT} shalw.d
endif

shalw_tmp.d: shalw_tmp.c
	     rm -f shalw_tmp.d
	     gcc ${GENOPT} shalw_tmp.c > shalw_tmp.d

shalw_tmp.c: shalw.d 
	     cp -f shalw.d shalw_tmp.c

clean:
	rm -f shalw shalw_tmp.c* shalw_tmp.d*

