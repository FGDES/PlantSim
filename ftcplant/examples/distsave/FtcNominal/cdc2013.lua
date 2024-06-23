--------------------------------------------------------------
--------------------------------------------------------------
-- Test abstractions for consistency  (experimental)
--
-- See "Interner Bericht Nr. 7 (2nd ed)", subsequently revised
-- for presentation at CDC2013.
--
-- Form a conceptual point of view "Interner Bericht Nr. 9" superseeds
-- the earlier results and has been presnted at WODES 2014 and are
-- implemented natively in libFAUDES (see IsStdSynthesisConsistent).
-- However, the earlier version implemented by the below Lua-code refers
-- to locally non-blocking control and can come in handy in this context.
--
-- Thomas Moor, 2012.
-- 
--------------------------------------------------------------
--------------------------------------------------------------

function faudes.IsConsistentAbstraction(gL,sCtrl,sObs)

-- Parameters
-- gL              plant model to test
-- sCtrl           controllable events
-- sObs            observable events
--
-- gL must be determinitsic and trim
-- alphabets muts be subsets of the alphabet from gL

 -- report
 faudes.Print(string.format("IsConsistentAbstraction(): #states=%d",gL:Size()))

 -- debug with consistent state indices
 gL:MinStateIndex()

 -- alphabets
 local sUCtrl = gL:Alphabet()-sCtrl
 local sUObs =  gL:Alphabet()-sObs

 -- projected loop
 local gLo=faudes.Generator()
 faudes.Project(gL,sObs,gLo)

 -- report
 faudes.Print(string.format("IsConsistentAbstraction(): #projection-states=%d",gLo:Size()))

 -- extended realisation incl. observer states
 local gLLo=faudes.Generator()
 local cmap=faudes.ProductCompositionMap()
 faudes.Parallel(gL,gLo,cmap,gLLo) 

 -- report
 faudes.Print(string.format("IsConsistentAbstraction(): #observer-states=%d",gLLo:Size()))

 -- debug files
 --[[
 gLLo:Write("tmp_labsdbg_llo.gen")
 gLo:Write("tmp_labsdbg_lo.gen")
 --]]

 -- ensure trim generators
 -- (not needed?)
 if not gLLo:IsTrim() then
   gLLo:Trim()
 end

 -- one-step target control operator (tests A,B and C)
 local function omegaABC(xM) 
   faudes.Print(string.format("IsConsistentAbstraction:omegaABC(): #target=%d",xM:Size()))
   local xRes=faudes.IndexSet()
   local resa=0
   local resb=0
   local resc=0
   local sit = gLLo:StatesBegin()
   while sit ~= gLLo:StatesEnd() do
     local pass=false
     local enevs= gLLo:ActiveEventSet(sit:Index())
     local uctrlenevs = enevs * sUCtrl
     local uobsenevs = enevs * sUObs
     local oenevs= enevs * sObs
     -- case A: unobservable event to target enabled
     if (not pass) and not uobsenevs:Empty() then
       local tit = gLLo:TransRelBegin(sit:Index())
       while tit~= gLLo:TransRelEnd(sit:Index()) do
         if xM:Exists(tit:X2()) and sUObs:Exists(tit:Ev()) then
           --print(sit:Index(), "pass case A:",enevs:ToString())
           xRes:Insert(sit:Index())
           resa=resa+1
           pass=true
           break
         end
       tit:Inc()
       end
     end     
     -- case B: uncontrollable event to target enabled
     if (not pass) and not uctrlenevs:Empty() then
       local tit = gLLo:TransRelBegin(sit:Index())
       while tit~= gLLo:TransRelEnd(sit:Index()) do
         if xM:Exists(tit:X2()) and sUCtrl:Exists(tit:Ev()) then
           --print(sit:Index(), "pass case B:",enevs:ToString())
           xRes:Insert(sit:Index())
           resb=resb+1
           pass=true
           break
         end
       tit:Inc()
       end
     end     
     -- case C: find consistent controllable and observable transitions to target
     if (not pass) and not oenevs:Empty() then
       local q=cmap:Arg1State(sit:Index())
       local z=cmap:Arg2State(sit:Index())
       local zenevs = gLo:ActiveEventSet(z)
       if zenevs <= sCtrl then
         pass=true
         local eit= zenevs:Begin()
         while eit ~= zenevs:End() do
           if not xM:Exists(gLLo:SuccessorState(sit:Index(),eit:Index())) then 
             pass=false
             break
           end
           eit:Inc()
         end
         if pass then
           --print(sit:Index(), "pass case C:",enevs:ToString())
           xRes:Insert(sit:Index())
           resc=resc+1
         end
       end
     end
     -- all cases failed
     if not pass then
       local enevs=gLLo:ActiveEventSet(sit:Index())
       local z=cmap:Arg2State(sit:Index())
       local zenevs = gLo:ActiveEventSet(z)
       --print(sit:Index(), "unresolved: ",enevs,zenevs)
     end
     -- iterate all states
     sit:Inc()
   end
   faudes.Print(string.format("IsConsistentAbstraction:omegaABC(): #resolved_A=%d",resa))
   faudes.Print(string.format("IsConsistentAbstraction:omegaABC(): #resolved_B=%d",resb))
   faudes.Print(string.format("IsConsistentAbstraction:omegaABC(): #resolved_C=%d",resc))
   faudes.Print(string.format("IsConsistentAbstraction:omegaABC(): #resolved_+=%d",xRes:Size()))
   return xRes:Copy()
 end


 -- star-step target control operator (test D)
 local function omegaD(xM) 
   faudes.Print(string.format("IsConsistentAbstraction:omegaD(): #target=%d",xM:Size()))
   local xRes=faudes.IndexSet()
   local sit = gLLo:StatesBegin()
   while sit ~= gLLo:StatesEnd() do
     local pass=false
     -- test for existence of transition to target
     local tit = gLLo:TransRelBegin(sit:Index())
     while tit~= gLLo:TransRelEnd(sit:Index()) do
       if xM:Exists(tit:X2()) then
         pass=true
         break
       end
       tit:Inc()
     end
     -- test for not allready in target
     if xM:Exists(sit:Index()) then
       pass=false
     end
     -- candidats not in target but with transition to target
     if pass then
       faudes.Print(string.format("IsConsistentAbstraction:omegaD(): testing state %d",sit:Index()))
       -- compute reachable qz-states outside the target
       local xReachQZ=faudes.IndexSet()
       xReachQZ:Insert(sit:Index())
       local todo=xReachQZ:Copy()
       while not todo:Empty() do
         --print("xReachQZ: todo", todo)
         local ins=faudes.IndexSet()
         local xit = todo:Begin()
         while xit~= todo:End() do
           local tit = gLLo:TransRelBegin(xit:Index())
           while tit~= gLLo:TransRelEnd(xit:Index()) do
             if (not xM:Exists(tit:X2())) and (not xReachQZ:Exists(tit:X2())) then
               ins:Insert(tit:X2())
             end
             tit:Inc()
           end
           xit:Inc()
         end
         xReachQZ:InsertSet(ins)
         todo=ins:Copy()
       end
       --xReachQZ:Write()
       -- compute reachable z-states with no corresponing qz-states in target
       local xReachZ=faudes.IndexSet()
       xReachZ:Insert(cmap:Arg2State(sit:Index()))
       local todo=xReachZ:Copy()
       while not todo:Empty() do
         --print("xReachZ: todo", todo)
         local ins=faudes.IndexSet()
         local zit = todo:Begin()
         while zit~= todo:End() do
           local tit = gLo:TransRelBegin(zit:Index())
           while tit~= gLo:TransRelEnd(zit:Index()) do
             if not xReachZ:Exists(tit:X2()) then
               local safe=true
               local xit = gLLo:StatesBegin()
               while xit~= gLLo:StatesEnd() do
                 if not xReachQZ:Exists(cmap:Arg2State(xit:Index())) then
                    safe=false
                    break
                 end
                 xit:Inc()
               end
               if safe then
                 ins:Insert(tit:X2())
               end
             end
             tit:Inc()
           end
           zit:Inc()
         end
         xReachZ:InsertSet(ins)
         todo=ins:Copy()
       end
       --xReachZ:Write()
       -- test whether z-component reaches marking
       pass = (xReachZ * gLo:MarkedStates()):Empty() 
     end
     -- record pass
     if pass then
       xRes:Insert(sit:Index())
     end
     -- iterate all states
     sit:Inc()
   end
   xRes:InsertSet(xM)
   faudes.Print(string.format("IsConsistentAbstraction:omegaD(): #resolved=%d",xRes:Size()))
   return xRes:Copy()
 end



 -- test locally complete control
 local xQZ = gLLo:States():Copy()
 local xOmegaQZ = omegaABC(xQZ)

 -- report unresolved
 if xQZ ~=xOmegaQZ then
   faudes.Print("IsConsistentAbstraction: failed test for locally complete control")
   local sit = gLLo:StatesBegin()
   while sit ~= gLLo:StatesEnd() do
     if not xOmegaQZ:Exists(sit:Index()) then
       local enevs=gLLo:ActiveEventSet(sit:Index())
       local z=cmap:Arg2State(sit:Index())
       local zenevs = gLo:ActiveEventSet(z)
       faudes.Print(string.format("unresolved state %d with enabled events %s",sit:Index(),enevs:ToString()))
       faudes.Print(string.format("  projection enabled events %s",zenevs:ToString()))
     end
     sit:Inc()
   end
   return false
 end
 faudes.Print("IsConsistentAbstraction: passed test for locally complete control")

 -- strict test for non-blocking control (as in "Interner Bericht Nr. 7 (1st ed.)")
 local xMi = gLLo:MarkedStates()
 local sz=0
 while xMi:Size() > sz do
   sz=xMi:Size()
   xMi = omegaABC(xMi) + xMi
 end
 if xQZ==xMi then
   faudes.Print("IsConsistentAbstraction: passed test for nonblocking control using operator omegaABC")
 else
   faudes.Print("IsConsistentAbstraction: failed test for nonblocking control using operator omegaABC")
 end

 -- relaxed test for non-blocking control (as in "Interner Bericht Nr. 7 (2nd ed.)", incl omegaD)
 if xQZ~=xMi then
   while true do
     sz=xMi:Size()
     xMi = omegaD(xMi)  
     if xMi:Size()==sz then break end
     while xMi:Size() > sz do
       sz=xMi:Size()
       xMi = omegaABC(xMi) + xMi
     end
     if xMi:Size()==xQZ:Size() then break end
   end
   if xMi:Size()==xQZ:Size() then
     faudes.Print("IsConsistentAbstraction: passed test for nonblocking control, iterating omegaABC and omegaD")
   end
 end

 -- report unresolved
 if xQZ~=xMi then
   faudes.Print("IsConsistentAbstraction: failed both tests for nonblocking control")
   local sit = gLLo:StatesBegin()
   while sit ~= gLLo:StatesEnd() do
     if not xMi:Exists(sit:Index()) then
       local enevs=gLLo:ActiveEventSet(sit:Index())
       local z=cmap:Arg2State(sit:Index())
       local q=cmap:Arg1State(sit:Index())
       local zenevs = gLo:ActiveEventSet(z)
       local tevs=faudes.EventSet()
       local eit= enevs:Begin()
       while eit ~= enevs:End() do
         if xMi:Exists(gLLo:SuccessorState(sit:Index(),eit:Index())) then 
            tevs:Insert(eit:Index())
         end
         eit:Inc()
       end
       if not tevs:Empty() then
         faudes.Print(string.format("unresolved state %d with enabled events %s",sit:Index(),enevs:ToString()))
         faudes.Print(string.format("  plant state id %d",q))
         faudes.Print(string.format("  projection enabled events %s",zenevs:ToString()))
         faudes.Print(string.format("  target enabled events %s",tevs:ToString()))
       end
     end
     sit:Inc()
   end
   return false
 end

 faudes.Print("IsConsistentAbstraction: all relevant tests passed")
 return true
end

--[[

-- abstraction demo on test data
print("=== abstraction demo 1: consistency tests")
ldemo = faudes.System("labsdemo1.gen")
sigmahi = ldemo:ObservableEvents()
sigmac = ldemo:ControllableEvents()
absok=faudes.IsConsistentAbstraction(ldemo,sigmac,sigmahi)
if absok then
 print("abstraction demo 1: consistency test passed (expected)")
else
 print("abstraction demo 1: consistency test FAILED (test case ERROR)")
end
obsok=faudes.IsNaturalObserver(ldemo,sigmahi)
if obsok then
 print("abstraction demo 1: natural observer test passed (test case ERROR)")
else
 print("abstraction demo 1: natural observer test failed (expected)")
end

-- abstraction demo on test data
print("=== abstraction demo 2: consistency tests")
ldemo = faudes.System("labsdemo2.gen")
sigmahi = ldemo:ObservableEvents()
sigmac = ldemo:ControllableEvents()
absok=faudes.IsConsistentAbstraction(ldemo,sigmac,sigmahi)
if absok then
 print("abstraction demo 2: consistency test passed (expected)")
else
 print("abstraction demo 2: consistency test FAILED (test case ERROR)")
end
obsok=faudes.IsNaturalObserver(ldemo,sigmahi)
if obsok then
 print("abstraction demo 2: natural observer test passed (test case ERROR)")
else
 print("abstraction demo 2: natural observer test failed (expected)")
end

--]]