
-- configure: number of conveyor-belts
n = 4




-- --------------------------------------------------
-- --------------------------------------------------
-- --------------------------------------------------
-- plant components
-- --------------------------------------------------
-- --------------------------------------------------
-- --------------------------------------------------

-- report
print("==============================================")
print("==============================================")
print(string.format("=== set up plant components for n=%d",n))

-- stack feeder and exit slide
g_sf  =  faudes.System("g_sf.gen")
g_xs  =  faudes.System("g_xs.gen")

-- converyor belts
g_cb={}
g_cbX =  faudes.System("g_cbX.gen")
for i=1,n do
  cbi = string.format("cb%d",i)
  g_cb[i] = g_cbX:Version("cbX",cbi)
  g_cb[i]:Name(cbi)
end

-- sync enter/exit
g_sf:EventRename("sf_exit", "sf_cb1_pass")
g_cb[1]:EventRename("cb1_enter", "sf_cb1_pass")
for i=1,n-1 do
  g_cb[i]:EventRename(string.format("cb%d_exit",i), string.format("cb%d_cb%d_pass",i,i+1) )
  g_cb[i+1]:EventRename(string.format("cb%d_enter",i+1), string.format("cb%d_cb%d_pass",i,i+1) )
end
g_cb[n]:EventRename(string.format("cb%d_exit",n), string.format("cb%d_xs_pass",n))
g_xs:EventRename("xs_enter", string.format("cb%d_xs_pass",n) )

-- debugging: dump components
g_sf:Write()
for i=1,n do
  g_cb[i]:Write()
end
g_xs:Write()


-- compose overall plant
g=faudes.System()
faudes.Parallel(g_sf, g_cb[1], g)
for i=2,n do
  faudes.Parallel(g, g_cb[i], g)
end
faudes.Parallel(g, g_xs,  g)


-- lift components to overall alphabet
sigma= g:Alphabet()
g_sf_full= faudes.Generator()
faudes.InvProject(g_sf,sigma,g_sf_full)
g_cb_full={}
for i=1,n do
  g_cb_full[i]= faudes.Generator() 
  faudes.InvProject(g_cb[i],sigma,g_cb_full[i])
end
g_xs_full= faudes.Generator()
faudes.InvProject(g_xs,sigma,g_xs_full)


-- report
g:Name("plant")
g:SWrite()


-- --------------------------------------------------
-- --------------------------------------------------
-- --------------------------------------------------
-- reduce plant
-- --------------------------------------------------
-- --------------------------------------------------
-- --------------------------------------------------

--[[

-- report
print("==============================================")
print("=== reduce plant")

-- sync events to remove
print("=== remove sync. events")
sigma_o = g:ObservableEvents()
sigma_o:Name("observable")
sigma_uo = g:Alphabet()-sigma_o
sigma_uo:Name("unobservable")
sigma_c= g:ControllableEvents()
sigma_c:Name("controllable")
sigma_c:Write()
sigma_uo:Write()


-- g:Trim()

ok=false
--ok=faudes.IsNaturalObserver(g,sigma_o)
--if ok then
--  print("natural observer condition satiesfied (???)")
--end
--ok=faudes.IsControlConsistentObserver(g,sigma_c,sigma_o)
--if not ok then
--  print("consistency condition satiesfied (???)")
--end

g:SWrite()
if not ok then
  print("sorry: cannot reduce")
else
  print("projecting to observable events")
  g:StateNamesEnabled(false)
  faudes.Project(g,sigma_o,g)
  g:SWrite()
end

g:Write("tmp_g.gen")


-- done
print("==============================================")

--]]


-- --------------------------------------------------
-- --------------------------------------------------
-- --------------------------------------------------
-- copmosition specification
-- --------------------------------------------------
-- --------------------------------------------------
-- --------------------------------------------------

--[[

-- report
print("==============================================")
print("==============================================")
print(string.format("=== set up composition specification for n=%d",n))

-- figure alphabets
sigma= g:Alphabet()
sigma_c= g:ControllableEvents()
sigma_c:Name("controllable")
sigma_c:Write()
sigma_f = g:ForcibleEvents()
sigma_f:Name("forcible")
sigma_f:Write()
sigma_p=faudes.EventSet()
sigma_p:Insert("sf_wpar")
sigma_p:Insert("xs_wpar")
for i=1,n do
  sigma_p:Insert(string.format("cb%d_wplv",i))
  sigma_p:Insert(string.format("cb%d_wpar",i))
end
sigma_p:Name("preemptable")
sigma_p:Write()


e_sf_cntr = faudes.Generator()
sigma_c = (sigma - g_sf:UncontrollableEvents()) + sigma_p
faudes.SupCon(g_sf_full,sigma_c,g,e_sf_cntr)
e_sf_cntr:SWrite()

e_cb_cntr={}
for i=1,n do
  e_cb_cntr[i]=faudes.Generator()
  sigma_c = (sigma - g_cb[i]:UncontrollableEvents()) + sigma_p
  faudes.SupCon(g_cb_full[i],sigma_c,g,e_cb_cntr[i])
  e_cb_cntr[i]:SWrite()
end

e_xs_cntr = faudes.Generator()
sigma_c = (sigma - g_xs:UncontrollableEvents()) + sigma_p
faudes.SupCon(g_xs_full, sigma_c, g,e_xs_cntr)
e_xs_cntr:SWrite()

e_cntr = faudes.Generator();
faudes.Parallel(e_sf_cntr,e_cb_cntr[1],e_cntr)
for i=2,n do
  faudes.Parallel(e_cntr, e_cb_cntr[i], e_cntr)
end
faudes.Parallel(e_cntr,e_xs_cntr,e_cntr)
e_cntr:Name("spec composition")

e_cntr:SWrite()

-- done
print("==============================================")

--]]


-- --------------------------------------------------
-- --------------------------------------------------
-- --------------------------------------------------
-- explicit specification
-- --------------------------------------------------
-- --------------------------------------------------
-- --------------------------------------------------

-- report
print("==============================================")
print(string.format("=== set up specifications for n=%d",n))

-- operator specification
e_op_get = faudes.Generator("e_op_get.gen")
e_op_put = faudes.Generator("e_op_put.gen")
e_op_lazy = faudes.Generator("e_op_lazy.gen")
e_op_lazy:InsEvent("sf_fdon")
e_op_lazy:SetTransition("B","sf_fdon","B")
e_op_lazy:InsEvent("sf_fdoff")
e_op_lazy:SetTransition("B","sf_fdoff","B")
for i=1,n do
  cbi_on=string.format("cb%d_bm+",i)
  e_op_lazy:InsEvent(cbi_on)
  e_op_lazy:SetTransition("B",cbi_on,"B")
  cbi_off=string.format("cb%d_boff",i)
  e_op_lazy:InsEvent(cbi_off)
  e_op_lazy:SetTransition("B",cbi_off,"B")
end


-- cooperative behaviour
e_sf_cb1 = faudes.Generator("e_sf_cb1.gen")
e_cbX_xs = faudes.Generator("e_cbX_xs.gen")
e_cbn_xs = e_cbX_xs:Version("cbX",string.format("cb%d",n))
e_cb_cb={}
e_cbX_cbY = faudes.Generator("e_cbX_cbY.gen")
for i=1,n-1 do
  e_cbi_cbY = e_cbX_cbY:Version("cbX",string.format("cb%d",i))
  e_cb_cb[i] = e_cbi_cbY:Version("cbY",string.format("cb%d",i+1))
end

-- lazy components
e_sf_lazy = faudes.Generator("e_sf_lazy.gen")
e_cb_lazy={}
e_cbX_cbY_lazy = faudes.Generator("e_cbX_cbY_lazy.gen")
for i=1,n-1 do
  cbx = string.format("cb%d",i)
  cby_wpar = string.format("cb%d_wpar",i+1)
  e_cb_lazy[i] = e_cbX_cbY_lazy:Version("cbX",cbx)
  e_cb_lazy[i]:EventRename("cbY_wpar",cby_wpar)
end
e_cbX_xs_lazy=faudes.Generator("e_cbX_xs_lazy.gen")
cbn = string.format("cb%d",n)
e_cb_lazy[n]= e_cbX_xs_lazy:Version("cbX",cbn)


-- write to file
e_sf_cb1:Write("tmp_e_sf_cb1.gen")
for i=1,n-1 do
  e_cb_cb[i]:Write(string.format("tmp_e_cb%d_cb%d.gen",i,i+1))
end
e_cbn_xs:Write(string.format("tmp_e_cb%d_xs.gen",n))
for i=1,n do
  e_cb_lazy[i]:Write(string.format("tmp_e_cb%d_lazy.gen",i))
end

-- dump for debugging
e_sf_cb1:Write()
for i=1,n-1 do
  e_cb_cb[i]:Write()
end
e_cbn_xs:Write()

-- compose
e = faudes.Generator()
faudes.Parallel(e_op_get,e_op_put,e)
faudes.Parallel(e,e_op_lazy,e)
faudes.Parallel(e,e_sf_cb1,e)
for i=1,n-1 do
  faudes.Parallel(e,e_cb_cb[i],e)
end
faudes.Parallel(e,e_cbn_xs,e)
-- faudes.Parallel(e,e_cntr,e)
faudes.Parallel(e,e_sf_lazy,e)
for i=1,n do
  faudes.Parallel(e,e_cb_lazy[i],e)
end

e:Name("specification")
e:SWrite()

-- done
print("==============================================")



-- --------------------------------------------------
-- --------------------------------------------------
-- --------------------------------------------------
-- synthesis
-- --------------------------------------------------
-- --------------------------------------------------
-- --------------------------------------------------

-- report
print("==============================================")
print("=== setting up alphabets")

-- all events
sigma = e:Alphabet() + g:Alphabet()

-- operator events
sigma_op = e:Alphabet() - g:Alphabet()

-- controllable events
sigma_c = g:ControllableEvents() + sigma_op
sigma_c:Name("controllable")
sigma_c:Write()

-- observable events
sigma_o = g:ObservableEvents() + sigma_op
sigma_o:Name("observable")
sigma_o:Write()

-- forcible events
sigma_f = g:ForcibleEvents()
--sigma_f:Insert("op_ack")
sigma_f:Name("forcible")
sigma_f:Write()

-- preemptable events
sigma_p=faudes.EventSet()
--sigma_p:Insert("xs_wplv")
--sigma_p:Insert("xs_wpar")
sigma_p:Insert("sf_wpar")
for i=1,n do
  --sigma_p:Insert(string.format("cb%d_wpar",i))
  sigma_p:Insert(string.format("cb%d_wplv",i))
end
--sigma_p:Insert(string.format("sf_cb1_pass",n))
for i=1,n-1 do
  --sigma_p:Insert(string.format("cb%d_cb%d_pass",i,i+1))
end
--sigma_p:Insert(string.format("cb%d_xs_pass",n))
sigma_p:Name("preemptable")
sigma_p:Write()


-- synthesis, nonblocking tdes semantics

print("=== synthesise tcontr. sublanguage incl. non-blocking")
faudes.InvProject(g,sigma)
faudes.InvProject(e,sigma)

--faudes.PrefixClosure(g)
--faudes.PrefixClosure(e)

if n>2 then
  g:StateNamesEnabled(false)
  e:StateNamesEnabled(false)
end
  
k = faudes.Generator()
faudes.SupTconNB(g,sigma_c, sigma_f, sigma_p,e,k)
k:SWrite()

-- synthesis incl normality

print("=== synthesise tcontr. sublanguage and prefix normal")

k0 = k:Copy()  -- faudes.Generator()
--faudes.Product(g,e,k0)
k0:Trim()
k0:StateNamesEnabled(false)
faudes.MarkAllStates(g)
k=k0:Copy()
repeat
  print("=== synthesis: iterate suptcon:", k:Size())
  k_recent=k:Copy()
  faudes.PrefixClosure(k)
  faudes.SupTconNB(g, sigma_c, sigma_f, sigma_p, k, k)
  print("=== synthesis: iterate supnorm:", k:Size())
  faudes.SupNormClosed(g,sigma_o,k,k)
  print("=== synthesis: iterate marking:", k:Size())
  faudes.Product(k,k0,k)
  k:Trim()
until faudes.LanguageEquality(k,k_recent)

kt=faudes.Generator()
if k:IsTrim() then 
  print("synthesis: trim test: OK")
end
faudes.SupTconNB(g, sigma_c, sigma_f, sigma_p, k, kt)
if faudes.LanguageEquality(k,kt) then
  print("synthesis: controllability test: OK")
end
kt=k:Copy()
faudes.PrefixClosure(kt)
if faudes.IsNormal(g,sigma_o,kt) then
  print("synthesis: normality test: OK")
end
if faudes.LanguageInclusion(k,e) then
  print("synthesis: inclusion spec test: OK")
end


k:SWrite()

k:Write(string.format("k%d.gen",n))
g:Write("tmp_g.gen")
e:Write("tmp_e.gen")

