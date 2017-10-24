(* ::Package:: *)

(* ::Input:: *)
(*(*Simulaci\[OAcute]n de protocolos*)*)
(*<< /home/docencia/RRT/drawTx.m*)


(* ::Input:: *)
(*<< /home/docencia/RRT/RandomData.m*)


(* ::Input:: *)
(*DrawWin[0,100,8](*inicio, ancho, capacidad de numeraci\[OAcute]n (representaci\[OAcute]n simplificada)*)*)


(* ::Input:: *)
(*SetIniParDraw[.5,.03];*)
(*a={0,5000,0,0,0};*)
(*Show [DrawWin[0,5,8],DrawPacketTx[a]]*)
(**)


(* ::Input:: *)
(*lstPck = Table[{2+n*10,10,n+1,0,0},{n,0,9}];*)
(*Show[DrawWin[0,20,8],Map[DrawPacketTx[#]&,lstPck]]*)


(* ::Input:: *)
(*(*Vamos a experimentar con la librer\[IAcute]a gr\[AAcute]fica. Vamos a dibujar la transmisi\[OAcute]n de paquetes en una cola M/M/1. Utilizamos el set inicial para los tiempos de propagaci\[OAcute]n y los de ACK*)*)
(*lambda = 100;*)
(*mu = 10000; (*Mu en este caso es la longitud en bits, por la librer\[IAcute]a*)*)
(*nmax = 100;*)
(*iarrs= Table[RandomExp[lambda],{nmax}];*)
(*servtimes = Table[RandomExp[mu],{nmax}];*)
(*arrs = Accumulate[iarrs];*)
(*FifoSchedulling[arrivals_, service_]:=Module[{n, checktime}, n =1; checktime = arrivals[[1]]; Map[(If[checktime >= #, checktime += service[[n++]], checktime = # + service[[n++]]])&, arrivals]];*)
(*depTimes = FifoSchedulling[arrs,servtimes];*)
(*injtimes =depTimes-servtimes;*)
(*(*Construyamos los paquetes*)*)
(*generatePList[arr_, stimes_]:=Module[{nsec},*)
(*MapThread[]]*)
