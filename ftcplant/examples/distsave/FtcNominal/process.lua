-- synthesis script for bottling process

-- configure
faudes.StateNamesOff()

-- cdc2013 demo, incl fnct def.
-- dofile('cdc2013.lua')

--------------------------------------------------------------
--------------------------------------------------------------
-- CreateQueue
--------------------------------------------------------------
--------------------------------------------------------------

-- build automaton to represent a fifo-buffer 
--
-- pstr: prefix of put-event labels
-- tstr: prefix of take-event label
-- estr: error-event label (empty string for no error)
-- m: number of symbols to put/take (max. 9, i.e., one digit)
-- n: depth of buffer
--
-- example: gen=CreateQueue("P","D","I",3,2) for a depth two
-- buffer of product requests P1,P2,P3 and respective processes 
-- D1,D2,D3. On buffer overflow, any further P event is effectively
-- ignored, except formal acknowledged by the error indicator I


function CreateQueue(pstr,tstr,estr,m,n)
  
  -- repare result
  local gRes=faudes.Generator()
  if estr~="" then gRes:InsEvent(estr) end

  -- have initial state with empty fifo in todo list
  local todo={"s"}
  gRes:InsInitState("s")
  gRes:SetMarkedState("s")

  -- work on todo list
  while # todo > 0 do
    -- get state name, aka encoded fifo state
    local x1str = todo[1]
    table.remove(todo,1)

-- append any put-events
    for j=1,m do
      -- case 1: can put
      if string.len(x1str)-1<n then
        local x2str = x1str .. j
        table.insert(todo,x2str)
        gRes:InsState(x2str)
        local evstr = pstr .. j
        gRes:InsEvent(evstr)
        gRes:SetTransition(x1str, evstr, x2str)        
      end
      -- case 2: overflow error
      if string.len(x1str)-1==n and estr~="" then
        local x2str = x1str .. "e"
        if not gRes:ExistsState(x2str) then gRes:InsState(x2str) end
        local evstr = pstr .. j
        gRes:InsEvent(evstr)
        gRes:SetTransition(x1str, evstr, x2str)        
        gRes:SetTransition(x2str, estr, x1str)        
      end
      -- case 3: can take
      if string.len(x1str)>1 then
        local x2str = "s" .. string.sub(x1str,3)  
        local evstr= tstr .. string.sub(x1str,2,2)
        gRes:InsEvent(evstr)
        gRes:SetTransition(x1str, evstr, x2str)        
      end
    end
  end

  -- pass on result
  return gRes:Copy()
end


-- generate small examples for documentation
--[[
reqbuf=CreateQueue("P","D","PI",2,2)
reqbuf:Write("reqbuf_docu.gen")
ackbuf=CreateQueue("D","p","",2,2)
ackbuf:Write("ackbuf_docu.gen")
--]]

-----------------------------------
-----------------------------------
-- transport plant incl process
-----------------------------------
-----------------------------------

-- report
print("=== plant models")

-- alternative transport models (see FtcTransport)
ltransf=faudes.System("htransport.gen");
ltransa=faudes.System("htransabs.gen");
ltranss=faudes.System("ltrans.gen");

-- choose
--print("=== using full transport model")
--lproduct=ltransf
print("=== using abstract transport model")
lproduct=ltransa
--print("=== using simple transport model")
--lproduct=ltranss

-- merge with process model (shuffle)
lproc=faudes.System("lproc.gen")
faudes.Parallel(lproduct,lproc,lproduct)

-- report
lproduct:Name("lproduct")
lproduct:SWrite()


-----------------------------------
-----------------------------------
-- synthesis for product
-----------------------------------
-----------------------------------

-- report
print("=== synthesis: product")

-- load
eproca=faudes.Generator("eproca.gen")
eprocb=faudes.Generator("eprocb.gen")

-- merge specs
eproc=faudes.Generator()
faudes.Parallel(eproca,eprocb,eproc)

-- synthesis
sigma= lproduct:Alphabet() + eproc:Alphabet()
sigmac = lproduct:ControllableEvents() + (sigma - lproduct:Alphabet())
sigmac:Name("sigmac")
sigmac:Write()
faudes.InvProject(lproduct,sigma)
faudes.InvProject(eproc,sigma)
lproduct:SetControllable(sigmac)
kproduct=faudes.System()
faudes.SupConNB(lproduct,eproc,kproduct)
faudes.StateMin(kproduct)
kproduct:Name("kproduct")

-- report
kproduct:SWrite()
kproduct:Write("tmp_kproduct.gen")

-- abstraction
print("=== abstraction: do project")
sigmahi=faudes.Alphabet();
sigmahi:Name("sigmahi")
sigmahi:FromString('<A> S2 R D1 D2 D3 S3</A>')
sigmahi:Write()
hproduct=faudes.System()
faudes.Project(kproduct,sigmahi,hproduct)
faudes.StateMin(hproduct)
hproduct:Name("hproduct")
hproduct:SWrite()
hproduct:Write("tmp_hproduct.gen")
print("=== abstraction: test natural observer property")
obsok=faudes.IsNaturalObserver(kproduct,sigmahi)
if obsok then
  print("=== first guess passed natural observer test (expected)")
else
  print("=== first guess failed natural observer test (ERROR)")
end
print("=== abstraction: test consistency (for demo only: this is implied by nat.obs)")
absok=faudes.IsStdSynthesisConsistent(kproduct,sigmac,hproduct)
--absok=faudes.IsConsistentAbstraction(kproduct,sigmac,sigmahi)
if absok then
  print("=== first guess passed consistency test (expected)")
else
  print("=== first guess failed consistency test (ERROR)")
end


-----------------------------------
-----------------------------------
-- synthesis for acknowledgement
-----------------------------------
-----------------------------------

-- report
print("=== synthesis: acknowledge")

-- generate buffer
eackbuf=CreateQueue("D","p","",3,4)
eackbuf:Name("eackbuf")
eackbuf:Write("tmp_eackbuf.gen")
eackbuf:SWrite()
eackex=faudes.Generator("eackex.gen")
eackspec=faudes.Generator()
faudes.Parallel(eackex,eackbuf,eackspec)

-- synthesis
sigma= hproduct:Alphabet() + eackspec:Alphabet()
sigmac = hproduct:ControllableEvents() + (sigma - hproduct:Alphabet())
sigmac:Name("sigmac")
sigmac:Write()
faudes.InvProject(hproduct,sigma)
faudes.InvProject(eackspec,sigma)
hproduct:SetControllable(sigmac)
kacknow=faudes.System()
faudes.SupConNB(hproduct,eackspec,kacknow)
kacknow:Name("kacknow")

-- report
kacknow:SWrite()
kacknow:Write("tmp_kacknow.gen")

-- abstract with control consistent observer
print("=== abstraction: using consistency test")
sigmahi=faudes.Alphabet();
sigmahi:FromString('<A> S2 D1 D2 D3</A>')
sigmahi:Name("sigmahi")
sigmahi:Write()
print("=== abstraction: do project")
faudes.StateMin(kacknow)
hacknow=faudes.System(kacknow)
faudes.Project(kacknow,sigmahi,hacknow)
faudes.StateMin(hacknow)
print("=== abstraction: test consistency")
absok=faudes.IsStdSynthesisConsistent(kacknow,sigmac,hacknow)
-- absok=faudes.IsConsistentAbstraction(kacknow,sigmac,sigmahi)
if absok then
  print("abstraction: consistency test: passed")
else
  print("abstraction: consistency test: FAILED")
end
hacknow:Name("hacknow")
hacknow:SWrite()
hacknow:Write("tmp_hacknow.gen")


-- abstract with natural observe
print("=== abstraction: construct natural observer")
sigmahi=faudes.Alphabet();
sigmahi:FromString('<A> D1 D2 D3 </A>')
faudes.NaturalObserverExtension(kacknow,sigmahi)
sigmahi:Name("sigmahi")
sigmahi:Write()


-----------------------------------
-----------------------------------
-- synthesis for requests
-----------------------------------
-----------------------------------

-- report
print("=== synthesis: requests")

-- generate buffer
ereqbuf=CreateQueue("P","D","I",3,4)
ereqbuf:SWrite()
ereqbuf:Write("tmp_ereqbuf.gen")

-- merge plant with random request model (need "I" forcible for this to work)
-- lreq=faudes.System("lreq.gen")
-- faudes.Parallel(hacknow,lreq,hacknow)

-- synthesis
sigma= hacknow:Alphabet() + ereqbuf:Alphabet()
sigma:Name("sigma")
sigma:Write()
sigmac = hacknow:ControllableEvents() + (sigma - hacknow:Alphabet())
sigmac:Name("sigmac")
sigmac:Write()
faudes.InvProject(hacknow,sigma)
faudes.InvProject(ereqbuf,sigma)
hacknow:SetControllable(sigmac)
krequest=faudes.System()
faudes.SupConNB(hacknow,ereqbuf,krequest)
krequest:Name("krequest")

-- report
krequest:SWrite()
krequest:Write("tmp_krequest.gen")


