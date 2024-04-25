c
c			
c
	SUBROUTINE t1vold_abc(f,S,a,b,c,
     +      a00_1,a00_2,a10_1,a10_2,a11_1,a11_2,a11_3,
     +      a12_1,a12_2,a12_3,a20_1,a20_2,a22_1,
     +      b00_1,b00_2,b10_1,b10_2,b11_1,b11_2,b11_3,
     +      b12_1,b12_2,b12_3,b20_1,b20_2,b22_1,
     +      c00,c10,c11,c12,c20,c22)
c
	double precision X(30), Y(30), Y2(30)
c
	double precision YY20(30),YY21(30), YY22(30),  YY23(30),YY24(30) 
	double precision YY25(30),YY26(30), YY27(30),  YY28(30),YY29(30)
	double precision YY30(30),YY31(30), YY32(30),  YY33(30),YY34(30)
c
	double precision ax00_1(30),ax00_2(30), ax10_1(30), ax10_2(30)
	double precision ax11_1(30)
	double precision ax11_2(30),ax11_3(30), ax12_1(30), ax12_2(30)
	double precision  ax12_3(30)
        double precision ax20_1(30),ax20_2(30), ax22_1(30)                    
	double precision bx00_1(30),bx00_2(30), bx10_1(30), bx10_2(30)
	double precision  bx11_1(30)
	double precision bx11_2(30),bx11_3(30), bx12_1(30), bx12_2(30)
	double precision  bx12_3(30)
	double precision bx20_1(30),bx20_2(30), bx22_1(30)
	double precision cx00(30),cx10(30), cx11(30), cx12(30),cx20(30)
	double precision  cx22(30)
c
	double precision ay00_1(30),ay00_2(30), ay10_1(30), ay10_2(30)
	double precision  ay11_1(30)
	double precision ay11_2(30),ay11_3(30), ay12_1(30), ay12_2(30)
	double precision  ay12_3(30)
	double precision ay20_1(30),ay20_2(30), ay22_1(30)
	double precision by00_1(30),by00_2(30), by10_1(30), by10_2(30)
	double precision  by11_1(30)
	double precision by11_2(30),by11_3(30), by12_1(30), by12_2(30)
	double precision  by12_3(30)
	double precision by20_1(30),by20_2(30), by22_1(30)
	double precision cy00(30),cy10(30), cy11(30), cy12(30),cy20(30)
	double precision  cy22(30)
c
c
	double precision a00_1(2,30),a00_2(2,30),a10_1(2,30),a10_2(2,30) 

	double precision a11_1(2,30)
	double precision a11_2(2,30),a11_3(2,30),a12_1(2,30),a12_2(2,30)
	double precision a12_3(2,30)
        double precision a20_1(2,30),a20_2(2,30), a22_1(2,30) 
	double precision b00_1(2,30),b00_2(2,30),b10_1(2,30),b10_2(2,30)
	double precision b11_1(2,30)
	double precision b11_2(2,30),b11_3(2,30),b12_1(2,30),b12_2(2,30)
	double precision b12_3(2,30)
	double precision b20_1(2,30), b20_2(2,30), b22_1(2,30)
	double precision c00(2,30), c10(2,30), c11(2,30), c12(2,30) 
	double precision c20(2,30), c22(2,30)
c
	double precision a(10,10,10), b(10,10,10), c(10,10)
	double precision S,f
c
	INTEGER ALFA
c
c	print *,S,f
	Szz = S
	np1 = 0
c	YP1 = 1e31
c	YPn = 1e31
	YP1 = 1.0
	YPn = 1.0
c	type *,'a(1,1,1) in=',a(1,1,1)
c---------------------------------------------------------
	np1 = a00_1(1,1)
	do 151 i=2,np1+1
	   ax00_1(i-1) = a00_1(1,i)
	   ay00_1(i-1) = a00_1(2,i)
151 	continue
	CALL SPLINE(ax00_1,ay00_1,np1,YP1,YPn,YY20)
	CALL SPLINT(ax00_1,ay00_1,YY20,np1,Szz,yy)
	a(1,3,1) = yy
c---------------------------------------------------------
	np1 = a00_2(1,1)
	do 152 i=2,np1+1
	   ax00_2(i-1) = a00_2(1,i)
	   ay00_2(i-1) = a00_2(2,i)
152	continue
	CALL SPLINE(ax00_2,ay00_2,np1,YP1,YPn,YY21)
	CALL SPLINT(ax00_2,ay00_2,YY21,np1,Szz,yy)
	a(1,3,2) = yy
c---------------------------------------------------------
	np1 = a10_1(1,1)
	do 153 i=2,np1+1
	   ax10_1(i-1) = a10_1(1,i)
	   ay10_1(i-1) = a10_1(2,i)
153	    continue
	CALL SPLINE(ax10_1,ay10_1,np1,YP1,YPn,YY22)
	CALL SPLINT(ax10_1,ay10_1,YY22,np1,Szz,yy)
	a(2,3,1) = yy
c---------------------------------------------------------
	np1=a10_2(1,1)
	do 154 i=2,np1+1
	   ax10_2(i-1) = a10_2(1,i)
	   ay10_2(i-1) = a10_2(2,i)
154	continue
	CALL SPLINE(ax10_2,ay10_2,17,YP1,YPn,YY23)
	CALL SPLINT(ax10_2,ay10_2,YY23,17,Szz,yy)
        a(2,3,2) = yy
c---------------------------------------------------------
	np1 = a11_1(1,1)
	do 155 i=2,np1+1
	   ax11_1(i-1) = a11_1(1,i)
	   ay11_1(i-1) = a11_1(2,i)
155	continue
	CALL SPLINE(ax11_1,ay11_1,np1,YP1,YPn,YY24)
	CALL SPLINT(ax11_1,ay11_1,YY24,np1,Szz,yy)
        a(2,4,1) = yy
	a(2,2,1) = yy
c---------------------------------------------------------
	np1 = a11_2(1,1)
	do 156 i=2,np1+1
	   ax11_2(i-1) = a11_2(1,i)
	   ay11_2(i-1) = a11_2(2,i)
156	continue
	CALL SPLINE(ax11_2,ay11_2,np1,YP1,YPn,YY25)
	CALL SPLINT(ax11_2,ay11_2,YY25,np1,Szz,yy)
        a(2,4,2) = yy
	a(2,2,2) = yy
c---------------------------------------------------------
	np1 = a11_3(1,1)
	do 157 i=2,np1+1
	   ax11_3(i-1) = a11_3(1,i)
	   ay11_3(i-1) = a11_3(2,i)
157	continue
	CALL SPLINE(ax11_3,ay11_3,np1,YP1,YPn,YY26)
	CALL SPLINT(ax11_3,ay11_3,YY26,np1,Szz,yy)
	a(2,4,3)=yy
	a(2,2,3)=yy
c---------------------------------------------------------
	np1=a12_1(1,1)
	do 158 i=2,np1+1
	   ax12_1(i-1)=a12_1(1,i)
	   ay12_1(i-1)=a12_1(2,i)
158	continue
	CALL SPLINE(ax12_1,ay12_1,np1,YP1,YPn,YY27)
	CALL SPLINT(ax12_1,ay12_1,YY27,np1,Szz,yy)
        a(2,5,1)=yy
	a(2,1,1)=yy
c---------------------------------------------------------
	np1=a12_2(1,1)
	do 159 i=2,np1+1
	   ax12_2(i-1)=a12_2(1,i)
	   ay12_2(i-1)=a12_2(2,i)
159	continue
	CALL SPLINE(ax12_2,ay12_2,np1,YP1,YPn,YY28)
	CALL SPLINT(ax12_2,ay12_2,YY28,np1,Szz,yy)
        a(2,5,2)=yy
	a(2,1,2)=yy
c---------------------------------------------------------
	np1=a12_3(1,1)
	do 161 i=2,np1+1
	   ax12_3(i-1)=a12_3(1,i)
	   ay12_3(i-1)=a12_3(2,i)
161	continue
	CALL SPLINE(ax12_3,ay12_3,np1,YP1,YPn,YY29)
	CALL SPLINT(ax12_3,ay12_3,YY29,np1,Szz,yy)
        a(2,5,3)=yy
	a(2,1,3)=yy
c---------------------------------------------------------
	np1=a20_1(1,1)
	do 162 i=2,np1+1
	   ax20_1(i-1)=a20_1(1,i)
	   ay20_1(i-1)=a20_1(2,i)
162	continue
	CALL SPLINE(ax20_1,ay20_1,np1,YP1,YPn,YY30)
	CALL SPLINT(ax20_1,ay20_1,YY30,np1,Szz,yy)
        a(3,3,1)=yy
c---------------------------------------------------------
	np1=a20_2(1,1)
	do 163 i=2,np1+1
	   ax20_2(i-1)=a20_2(1,i)
	   ay20_2(i-1)=a20_2(2,i)
163	continue
	CALL SPLINE(ax20_2,ay20_2,np1,YP1,YPn,YY31)
	CALL SPLINT(ax20_2,ay20_2,YY31,np1,Szz,yy)
        a(3,3,2)=yy
c---------------------------------------------------------
	np1=a22_1(1,1)
	do 164 i=2,np1+1
	   ax22_1(i-1)=a22_1(1,i)
	   ay22_1(i-1)=a22_1(2,i)
164	continue
	CALL SPLINE(ax22_1,ay22_1,np1,YP1,YPn,YY32)
	CALL SPLINT(ax22_1,ay22_1,YY32,np1,Szz,yy)
        a(3,5,1)=yy
	a(3,1,1)=yy
c---------------------------------------------------------
c---------------------------------------------------------
	np1=b00_2(1,1)
	do 172 i=2,np1+1
	   bx00_2(i-1)=b00_2(1,i)
	   by00_2(i-1)=b00_2(2,i)
172	continue
	CALL SPLINE(bx00_2,by00_2,np1,YP1,YPn,YY34)
	CALL SPLINT(bx00_2,by00_2,YY34,np1,Szz,yy)
        b(1,3,2)=yy
c---------------------------------------------------------
	np1=b00_1(1,1)
	do 171 i=2,np1+1
	   bx00_1(i-1)=b00_1(1,i)
	   by00_1(i-1)=b00_1(2,i)
171	continue
	CALL SPLINE(bx00_1,by00_1,np1,YP1,YPn,YY33)
	CALL SPLINT(bx00_1,by00_1,YY33,np1,Szz,yy)
        b(1,3,1)=yy
c---------------------------------------------------------
	np1=b10_1(1,1)
	do 173 i=2,np1+1
	   bx10_1(i-1)=b10_1(1,i)
	   by10_1(i-1)=b10_1(2,i)
173	continue
	CALL SPLINE(bx10_1,by10_1,np1,YP1,YPn,Y2)
	CALL SPLINT(bx10_1,by10_1,Y2,np1,Szz,yy)
        b(2,3,1)=yy
c---------------------------------------------------------
	np1=b10_2(1,1)
	do 174 i=2,np1+1
	   bx10_2(i-1)=b10_2(1,i)
	   by10_2(i-1)=b10_2(2,i)
174	continue
	CALL SPLINE(bx10_2,by10_2,np1,YP1,YPn,Y2)
	CALL SPLINT(bx10_2,by10_2,Y2,np1,Szz,yy)
        b(2,3,2)=yy
c---------------------------------------------------------
	np1=b11_1(1,1)
	do 175 i=2,np1+1
	   bx11_1(i-1)=b11_1(1,i)
	   by11_1(i-1)=b11_1(2,i)
175	continue
	CALL SPLINE(bx11_1,by11_1,np1,YP1,YPn,Y2)
	CALL SPLINT(bx11_1,by11_1,Y2,np1,Szz,yy)
        b(2,4,1)=yy
	b(2,2,1)=yy
c---------------------------------------------------------
	np1=b11_2(1,1)
	do 176 i=2,np1+1
	   bx11_2(i-1)=b11_2(1,i)
	   by11_2(i-1)=b11_2(2,i)
176	continue
	CALL SPLINE(bx11_2,by11_2,np1,YP1,YPn,Y2)
	CALL SPLINT(bx11_2,by11_2,Y2,np1,Szz,yy)
        b(2,4,2)=yy
	b(2,2,2)=yy
c---------------------------------------------------------
	np1=b11_3(1,1)
	do 177 i=2,np1+1
	   bx11_3(i-1)=b11_3(1,i)
	   by11_3(i-1)=b11_3(2,i)
177	continue
	CALL SPLINE(bx11_3,by11_3,np1,YP1,YPn,Y2)
	CALL SPLINT(bx11_3,by11_3,Y2,np1,Szz,yy)
        b(2,4,3)=yy
	b(2,2,3)=yy
c---------------------------------------------------------
	np1=b12_1(1,1)
	do 178 i=2,np1+1
	   bx12_1(i-1)=b12_1(1,i)
	   by12_1(i-1)=b12_1(2,i)
178	continue
	CALL SPLINE(bx12_1,by12_1,np1,YP1,YPn,Y2)
	CALL SPLINT(bx12_1,by12_1,Y2,np1,Szz,yy)
        b(2,5,1)=yy
	b(2,1,1)=yy
c---------------------------------------------------------
	np1=b12_2(1,1)
	do 179 i=2,np1+1
	   bx12_2(i-1)=b12_2(1,i)
	   by12_2(i-1)=b12_2(2,i)
179	continue
	CALL SPLINE(bx12_2,by12_2,np1,YP1,YPn,Y2)
	CALL SPLINT(bx12_2,by12_2,Y2,np1,Szz,yy)
        b(2,5,2)=yy
	b(2,1,2)=yy
c---------------------------------------------------------
	np1=b12_3(1,1)
	do 181 i=2,np1+1
	   bx12_3(i-1)=b12_3(1,i)
	   by12_3(i-1)=b12_3(2,i)
181	continue
	CALL SPLINE(bx12_3,by12_3,np1,YP1,YPn,Y2)
	   CALL SPLINT(bx12_3,by12_3,Y2,np1,Szz,yy)
        b(2,5,3)=yy
	b(2,1,3)=yy
c---------------------------------------------------------
	np1=b20_1(1,1)
	do 182 i=2,np1+1
	   bx20_1(i-1)=b20_1(1,i)
	   by20_1(i-1)=b20_1(2,i)
182	continue
	CALL SPLINE(bx20_1,by20_1,np1,YP1,YPn,Y2)
	CALL SPLINT(bx20_1,by20_1,Y2,np1,Szz,yy)
        b(3,3,1)=yy
c---------------------------------------------------------
	np1=b20_2(1,1)
	do 183 i=2,np1+1
	   bx20_2(i-1)=b20_2(1,i)
	   by20_2(i-1)=b20_2(2,i)
183	continue
	CALL SPLINE(bx20_2,by20_2,np1,YP1,YPn,Y2)
	CALL SPLINT(bx20_2,by20_2,Y2,np1,Szz,yy)
        b(3,3,2)=yy
c---------------------------------------------------------
	np1=b22_1(1,1)
	do 184 i=2,np1+1
	   bx22_1(i-1)=b22_1(1,i)
	   by22_1(i-1)=b22_1(2,i)
184	continue
	CALL SPLINE(bx22_1,by22_1,np1,YP1,YPn,Y2)
	CALL SPLINT(bx22_1,by22_1,Y2,np1,Szz,yy)
        b(3,5,1)=yy
	b(3,1,1)=yy
c---------------------------------------------------------
c---------------------------------------------------------
	np1=c00(1,1)
	do 191 i=2,np1+1
	   cx00(i-1)=c00(1,i)
	   cy00(i-1)=c00(2,i)
191	continue
	CALL SPLINE(cx00,cy00,np1,YP1,YPn,Y2)
	CALL SPLINT(cx00,cy00,Y2,np1,Szz,yy)
        c(1,3)=yy
c---------------------------------------------------------
	np1=c10(1,1)
	do 192 i=2,np1+1
	   cx10(i-1)=c10(1,i)
	   cy10(i-1)=c10(2,i)
192	continue
	CALL SPLINE(cx10,cy10,np1,YP1,YPn,Y2)
	CALL SPLINT(cx10,cy10,Y2,np1,Szz,yy)
        c(2,3)=yy
c---------------------------------------------------------
	np1=c11(1,1)
	do 193 i=2,np1+1
	   cx11(i-1)=c11(1,i)
	   cy11(i-1)=c11(2,i)
193	continue
	CALL SPLINE(cx11,cy11,np1,YP1,YPn,Y2)
	CALL SPLINT(cx11,cy11,Y2,np1,Szz,yy)
        c(2,4)=yy
	c(2,2)=yy
c---------------------------------------------------------
	np1=c12(1,1)
	do 194 i=2,np1+1
	   cx12(i-1)=c12(1,i)
	   cy12(i-1)=c12(2,i)
194	continue
	CALL SPLINE(cx12,cy12,np1,YP1,YPn,Y2)
	CALL SPLINT(cx12,cy12,Y2,np1,Szz,yy)
        c(2,5)=yy
	c(2,1)=yy
c---------------------------------------------------------
	np1=c20(1,1)
	do 195 i=2,np1+1
	   cx20(i-1)=c20(1,i)
	   cy20(i-1)=c20(2,i)
195	continue
	CALL SPLINE(cx20,cy20,np1,YP1,YPn,Y2)
	CALL SPLINT(cx20,cy20,Y2,np1,Szz,yy)
        c(3,3)=yy
c---------------------------------------------------------
	np1=c22(1,1)
	do 196 i=2,np1+1
	   cx22(i-1)=c22(1,i)
	   cy22(i-1)=c22(2,i)
196	continue
	CALL SPLINE(cx22,cy22,np1,YP1,YPn,Y2)
	CALL SPLINT(cx22,cy22,Y2,np1,Szz,yy)
        c(3,5)=yy
	c(3,1)=yy
c---------------------------------------------------------
c     ola     ........ fim Spline/Splint ........
c---------------------------------------------------------
c
c	type *,'a(1,1,1)=',a(1,1,1)
c	do 1000 ii=1,10
c	   do 1001 jj=1,10
c             do 1002 kk=1,10
c	type *,'a=',a(ii,jj,kk)
c1002          continue
c1001       continue
c1000	continue
	RETURN
	END 
c
c
c
      SUBROUTINE SPLINE(X,Y,N,YP1,YPN,Y2)
      PARAMETER (NMAX=100)
      double precision X(N),Y(N),Y2(N),U(NMAX)
      IF (YP1.GT..99E30) THEN
        Y2(1)=0.
        U(1)=0.
      ELSE
        Y2(1)=-0.5
        U(1)=(3./(X(2)-X(1)))*((Y(2)-Y(1))/(X(2)-X(1))-YP1)
      ENDIF
      DO 11 I=2,N-1
        SIG=(X(I)-X(I-1))/(X(I+1)-X(I-1))
        P=SIG*Y2(I-1)+2.
        Y2(I)=(SIG-1.)/P
        U(I)=(6.*((Y(I+1)-Y(I))/(X(I+1)-X(I))-(Y(I)-Y(I-1))
     *      /(X(I)-X(I-1)))/(X(I+1)-X(I-1))-SIG*U(I-1))/P
11    CONTINUE
      IF (YPN.GT..99E30) THEN
        QN=0.
        UN=0.
      ELSE
        QN=0.5
        UN=(3./(X(N)-X(N-1)))*(YPN-(Y(N)-Y(N-1))/(X(N)-X(N-1)))
      ENDIF
      Y2(N)=(UN-QN*U(N-1))/(QN*Y2(N-1)+1.)
      DO 12 K=N-1,1,-1
        Y2(K)=Y2(K)*Y2(K+1)+U(K)
12    CONTINUE
      RETURN
      END
c
c
c
      SUBROUTINE SPLINT(XA,YA,Y2A,N,X,Y)
      double precision XA(N),YA(N),Y2A(N)
      KLO=1
      KHI=N
c	print *,X
1     IF (KHI-KLO.GT.1) THEN
        K=(KHI+KLO)/2
        IF(XA(K).GT.X)THEN
          KHI=K
        ELSE
          KLO=K
        ENDIF
      GOTO 1
      ENDIF
      H=XA(KHI)-XA(KLO)
c      IF (H.EQ.0.) PAUSE 'Bad XA input.'
      A=(XA(KHI)-X)/H
      B=(X-XA(KLO))/H
      Y=A*YA(KLO)+B*YA(KHI)+
     *      ((A**3-A)*Y2A(KLO)+(B**3-B)*Y2A(KHI))*(H**2)/6.
c	print *,H,Y,A,B,YA(KHI),Y2A(KLO),Y2A(KHI),XA(KHI),XA(KLO),X
      RETURN
      END
