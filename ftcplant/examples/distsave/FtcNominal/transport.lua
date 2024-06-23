-- synthesis script for bottling plant transport

-- configure
faudes.StateNamesOff()

-----------------------------------
-----------------------------------
-- place 1
-----------------------------------
-----------------------------------

-- report
print("=== load models for place 1")

-- feed and roundabout control
lfeed=faudes.System("lfeed.gen");
lractrl=faudes.System("lractrl.gen")
lplace1=faudes.System()
faudes.Parallel(lfeed,lractrl,lplace1)

-- specifications
efeeda=faudes.Generator("efeeda.gen")
efeedb=faudes.Generator("efeedb.gen")
eplace1=faudes.Generator()
faudes.Parallel(efeeda,efeedb,eplace1)

-- report
lfeed:GraphWrite("tmp_lfeed.png")
lractrl:GraphWrite("tmp_lracntr.png")
lplace1:GraphWrite("tmp_lplace1.png")
efeeda:GraphWrite("tmp_efeeda.png")
efeedb:GraphWrite("tmp_efeedb.png")
eplace1:GraphWrite("tmp_eplace1.png")

-----------------------------------
-----------------------------------
-- place 2-7
-----------------------------------
-----------------------------------

-- report
print("=== building models for places 2-7")

lplace2=faudes.System("lplace2.gen")

lplacei=faudes.System("lplacei.gen")
lplacet=faudes.System()

lplace3=faudes.System()
faudes.Version(lplacei,"P","2",lplacet)
faudes.Version(lplacet,"T","3",lplace3)
lplace4=faudes.System()
faudes.Version(lplacei,"P","3",lplacet)
faudes.Version(lplacet,"T","4",lplace4)
lplace5=faudes.System()
faudes.Version(lplacei,"P","4",lplacet)
faudes.Version(lplacet,"T","5",lplace5)
lplace6=faudes.System()
faudes.Version(lplacei,"P","5",lplacet)
faudes.Version(lplacet,"T","6",lplace6)
lplace7=faudes.System()
faudes.Version(lplacei,"P","6",lplacet)
faudes.Version(lplacet,"T","7",lplace7)

lexiti=faudes.System("lexiti.gen")
lexit7=faudes.System()
faudes.Version(lexiti,"T","7",lexit7)

-----------------------------------
-----------------------------------
-- synthesis for 7 places
-----------------------------------
-----------------------------------

-- report
print("=== synthesis for places 1--7")

-- plant
lplant7p=faudes.System()
faudes.Parallel(lplace1,lplace2,lplant7p)
faudes.Parallel(lplant7p,lplace3,lplant7p)
faudes.Parallel(lplant7p,lplace4,lplant7p)
faudes.Parallel(lplant7p,lplace5,lplant7p)
faudes.Parallel(lplant7p,lplace6,lplant7p)
faudes.Parallel(lplant7p,lplace7,lplant7p)
faudes.Parallel(lplant7p,lexit7,lplant7p)
lplant7p:Name("lplant7p")
lplant7p:SWrite()
lplant7p:Write("tmp_ltransport.gen")

-- report
print("=== plant alphabet ")
lplant7p:Alphabet():Write()

-- spec
tmp=faudes.Generator("eexiti.gen")
esystem7p=faudes.Generator()
faudes.Version(tmp,"T","7",esystem7p)
tmp=faudes.Generator("eproci.gen")
eproc7p=faudes.Generator()
faudes.Version(tmp,"T","4",eproc7p)
faudes.Parallel(esystem7p,eproc7p,esystem7p)
elazy7p=faudes.Generator("elazy7.gen")
faudes.Parallel(esystem7p,elazy7p,esystem7p)
faudes.Parallel(esystem7p,eplace1,esystem7p)
esystem7p:Name("esystem7p")
esystem7p:SWrite()
esystem7p:Write("tmp_etransport.gen")

-- synth
sigma7p= esystem7p:Alphabet() + lplant7p:Alphabet()
faudes.InvProject(esystem7p,sigma7p)
faudes.InvProject(lplant7p,sigma7p)
lplant7p:InsControllableEvent("X")
kloop7p=faudes.System()
--faudes.SupRelativelyPrefixClosed(lplant7p,esystem7p,esystem7p)
faudes.SupConNormCmplNB(lplant7p,esystem7p,kloop7p)
kloop7p:ClrControllable("X")
kloop7p:Name("kloop7p")
kloop7p:SWrite()
htransport7p=faudes.System()
faudes.Project(kloop7p,kloop7p:ObservableEvents(),htransport7p)
faudes.StateMin(htransport7p)
htransport7p:Name("htransport7p")
htransport7p:SWrite()
htransport7p:Write("tmp_htransport7p.gen")



-- test for consitent abstraction
print("=== testing for consistent abstraction ")
sigma7phi=faudes.Alphabet();
sigma7phi:FromString('<A> X S3 S2 R</A>')
sigma7phi:Name("sigma7phi")
sigma7phi:Write()
htransabs7p=faudes.System()
htransabs7p:Name("htransabs7p")
faudes.Project(kloop7p,sigma7phi,htransabs7p)
faudes.StateMin(htransabs7p)
htransabs7p:SWrite()
absok=faudes.IsStdSynthesisConsistent(kloop7p,htransabs7p)
if absok then
  print("abstraction: consistency test: passed")
else
  print("abstraction: consistency test: FAILED")
end
htransabs7p:Write("tmp_htransabs7p.gen")


-- build natural observer abstraction
-- (expected high-level alphabet S1,S2,R,S3,X --- could also be V instead of S2)            
print("=== building natural observer abstraction ")
sigma7phi=faudes.Alphabet();
sigma7phi:FromString('<A> X S3 S1 R</A>')
faudes.NaturalObserverExtension(kloop7p,sigma7phi)
sigma7phi:Name("sigma7phi")
sigma7phi:Write()
htransabs7p=faudes.System()
faudes.Project(kloop7p,sigma7phi,htransabs7p)
faudes.StateMin(htransabs7p)
htransabs7p:SWrite()

