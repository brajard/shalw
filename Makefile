GENOPT= -E
YAOPT= +g -x shalw
#YAOBIN=${YAODIR}/etc/YaoI
#YAOBIN=${YAODIR}/etc/yao92
YAOBIN=./yao92

shalw:  shalw_tmp.d shalw.h renorm.h
	rm -f Y2shalw_tmp.h
	rm -f Y2shalw_tmp.h
	rm -f Yshalw_tmp.cpp
	${YAOBIN} ${YAOPT} shalw_tmp.d
	ln -s Yworkdir/Y2shalw_tmp.h
	ln -s Yworkdir/Yshalw_tmp.cpp

shalw_tmp.d: shalw_tmp.c
	     rm -f shalw_tmp.d
	     gcc ${GENOPT} shalw_tmp.c > shalw_tmp.d

shalw_tmp.c: shalw.d 
	     cp -f shalw.d shalw_tmp.c

clean:
	rm -f shalw_tmp.c* shalw_tmp.d*  

