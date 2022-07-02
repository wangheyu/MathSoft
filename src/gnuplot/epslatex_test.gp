set terminal epslatex
set output "epslatex_eg.tex"
x=.10; y=.15; dy=.05
set label "left torus:"				at screen x,y; y=y-dy
set label '$x=\cos u+\frac{1}{2}\cos u \cos v$'	at screen x,y; y=y-dy 
set label '$y=\sin u+\frac{1}{2}\sin u \cos v$' at screen x,y; y=y-dy 
set label "$z=\\frac{1}{2}\\sin v$"   	     	at screen x,y
x=.65; y=.08
set label '\parbox{2.5in}{right torus:\\$x=1+\cos u+\fr\
ac{1}{2}\cos u \cos v$\\$y=\frac{1}{2}\sin v$\\\
$z=\sin u + \frac{1}{2}\sin u \cos v$}' at screen x,y left
