c                                                            
	SUBROUTINE t1vold_new(f,delta,S,Tau_S,Tau_L,k_1,p_p,
     +      a,b,c,t1_ang,j0,j1,j2,a_fact0,a_fact1,a_fact2)
c
        DOUBLE PRECISION Szz,Dperp,Dpar,P,afact0,afact1,afact2,K1,K2,K3
	COMMON /BB/ Szz, Dperp, Dpar, K1, P, afact0, afact1, afact2
c
	DOUBLE PRECISION a(10,10,10), b(10,10,10), c(10,10)
	DOUBLE PRECISION cklal(10,10)
c
	DOUBLE PRECISION tau(10,10,10), vjota(3,100)
	DOUBLE PRECISION vj0(2), vj1(2), vj2(2), vjor(3,200,2)
	INTEGER ALFA
	DOUBLE PRECISION K_1, j0, j1, j2
        DOUBLE PRECISION f,delta,S,Tau_S,Tau_L,p_p,t1_ang
        DOUBLE PRECISION a_fact0,a_fact1,a_fact2
c
c        type *,'ola1'
	Szz    = S
	K1     = K_1
	P      = P_P
	Dperp  = 1.0/(6.0 * Tau_S) 
	Dpar   = 1.0/(6.0 * Tau_L) 	
	afact0 = a_fact0	
	afact1 = a_fact1	
	afact2 = a_fact2	
c
c        print *,f,delta,S,Tau_S
	do 400 k = 0, 2
	   kk = k + 1
	   if (k.eq.0) then 
	            ct2 = 2.0 / 3.0
	       else if (k.eq.1) then 
	            ct2 = 1.0 / 9.0
	       else if (k.eq.2) then 
	            ct2 = 4.0 / 9.0
	       else 
	   endif
	do 405 ko = 1, 2
	   ff = ko * f
	   w2 = (2.0 * 3.14159 * ff)**2
	   som = 0.0
c
	   do 410 l = -2, 2
	      ll = l + 3
	      sum = 0.0
c
	      do 420 alfa = 1, 3
	         if (k.eq.0 .and. l.eq.0 .and. alfa.ne.3) then
	                    goto 444
	            else if (k.eq.1 .and. l.eq.0 .and. alfa.ne.3) then
  	                    goto 444
	            else if (k.eq.1 .and. IABS(l).eq.1) then
	                    goto 444 
	            else if (k.eq.1 .and. IABS(l).eq.2) then
	                    goto 444
	            else if (k.eq.2 .and. l.eq.0 .and. alfa.ne.3) then
	                    goto 444
 	          else if (k.eq.2.and.IABS(l).eq.2.and.alfa.eq.1) then
	                    goto 444
	            else
	                    goto 420
	         endif
444	         continue
	         cklal(kk,ll) = c(kk,ll) * al(l)
	         if (l.eq.0) then 
	               tau(kk,ll,alfa) = b(kk,ll,alfa) * taul(l)
	            else 
	               tbb = b(kk,ll,alfa) * taul(l) * tl(l)
	               tcc = tbb / (b(kk,ll,alfa) * taul(l) + tl(l))
	               tau(kk,ll,alfa) = tcc
	         endif
	         aaa = 2.0 * tau(kk,ll,alfa)
	         aaa = aaa / (1.0 + w2 * tau(kk,ll,alfa)**2)
 	         aaa = aaa * ct2 * a(kk,ll,alfa)
	         sum = sum + aaa
420	      continue 
	   som = som + sum * cklal(kk,ll) 
410	   continue
	   vjota(kk,ko) = 6.406e11 * som
c	   aww = alog(w)
c          ajj = alog(vjota(kk,ko))
405	 continue
400	continue
c        type *,'ola f'
c
	do 331 ij = 1, 2
	   vj0(ij) = vjota(1,ij)
	   vj1(ij) = vjota(2,ij)
	   vj2(ij) = vjota(3,ij)
331	continue
	nnn = 1
	del  = delta * 3.14159236 / 180.0
	sen2 = (sin(del))**2
	sen4 = (sin(del))**4
c
	do 333 i = 1, 2
	      vjor(1,nnn,i) = (1.0-3.0*sen2+2.25*sen4) * vj0(i) + 
     + 18.0 * (sen2 - sen4) * vj1(i) + (9.0/8.0) * sen4 * vj2(i)
	      vjor(2,nnn,i) = 0.25 * (sen2 - sen4) * vj0(i) + 
     + (1.0-2.5*sen2+2.0*sen4)*vj1(i)+0.125*(2.0*sen2-sen4)*vj2(i) 
	   vjor(3,nnn,i)=0.25*sen4*vj0(i)+2.0*(2.0*sen2-sen4)*vj1(i)+ 
     + (1.0 - sen2 + 0.125*sen4) * vj2(i) 
c
c ---- Para um policristal: ------
c
c	vjor(2,nnn,i) = vj0(i)/30. + vj1(i)*0.4 + vj2(i)/10.0
c	vjor(3,nnn,i) = vj0(i)*(2./15.) + vj1(i)*(8./5.) + vj2(i)*0.4 
c
333	continue
	j0 = vjor(1,1,1)
	j1 = vjor(2,1,1)
	j2 = vjor(3,1,1)
	t1_ang = vjor(2,nnn,1) + vjor(3,nnn,2)
c
	RETURN
	END
c
c
c
	FUNCTION AL(l)
	COMMON /BB/ Szz, Dperp, Dpar, K1, P, afact0, afact1, afact2
        DOUBLE PRECISION Szz,Dperp,Dpar,P,afact0,afact1,afact2,K1,K2,K3
c
	if (l.eq.-2 .or. l.eq.2) then
	      al = afact2 * 3.0
	   else if (l.eq.-1 .or. l.eq.1) then
	      al = afact1 * 3.0
	   else if (l.eq.0) then
	      al = afact0 * 6.0
	   else 
	endif
c
	RETURN
	END 
c
c
c
	FUNCTION TL(l)
	COMMON /BB/ Szz, Dperp, Dpar, K1, P, afact0, afact1, afact2
        DOUBLE PRECISION Szz,Dperp,Dpar,P,afact0,afact1,afact2,K1,K2,K3
c
	K2 = (3.0 * P + 1.0) * K1
	if (l.eq.-2 .or. l.eq.2) then
	      tl = 1.0 / K2
	   else if (l.eq.-1 .or. l.eq.1) then
	      tl = 1.0 / K1
	   else 
	      tl = 0.0
	endif
c
	RETURN
	END 
c
c
c
	FUNCTION TAUL(l)
	COMMON /BB/ Szz, Dperp, Dpar, K1, P, afact0, afact1, afact2
        DOUBLE PRECISION Szz,Dperp,Dpar,P,afact0,afact1,afact2,K1,K2,K3
c
	if (l.eq.-2 .or. l.eq.2) then
	      taul = 1.0 / (2.0*Dperp + 4.0*Dpar)
	   else if (l.eq.-1 .or. l.eq.1) then
	      taul = 1.0 / (5.0*Dperp + Dpar)
	   else if (l.eq.0) then
	      taul = 1.0 / (6.0*Dperp)
	   else 
c	      type *,'Tau(l) nao definido'
	endif
c
	RETURN
	END 
