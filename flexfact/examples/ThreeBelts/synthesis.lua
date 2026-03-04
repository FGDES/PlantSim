-- synthesis.lua
--
-- Design a simple monolithic supervisor to operate 
-- four conveyor belts as a left-to-right transport system.

-- dont do name states
faudes.StateNamesOff();

-------------------------------------------------------------------------------
-- read files
-------------------------------------------------------------------------------

-- report
print("%%%%%%%%% synthesis: reading plant and specification data")

-- load plant components from file
plant_sf       = faudes.System("sf.gen")
plant_cbf1     = faudes.System("cbf1.gen")
plant_cb1      = faudes.System("cb1.gen")
plant_cb12     = faudes.System("cb12.gen")
plant_cb2      = faudes.System("cb2.gen")
plant_cb23     = faudes.System("cb23.gen")
plant_cb3      = faudes.System("cb3.gen")
plant_cb3x     = faudes.System("cb3x.gen")
plant_xs       = faudes.System("xs.gen")

-- load specification components from file
spec_cbf       = faudes.System("specf.gen")
spec_cb1       = faudes.System("spec1.gen")
spec_cb2       = faudes.System("spec2.gen")
spec_cb3       = faudes.System("spec3.gen")
spec_cbx       = faudes.System("specx.gen")
spec_tuf1      = faudes.System("specf1.gen")
spec_tu12      = faudes.System("spec12.gen")
spec_tu23      = faudes.System("spec23.gen")
spec_tu3x      = faudes.System("spec3x.gen")



-------------------------------------------------------------------------------
-- compose overall plant and specification
-------------------------------------------------------------------------------

-- report
print("%%%%%%%%% synthesis: compose plant and specification")

-- build plant
plant_all = faudes.System()
faudes.Parallel(plant_sf,plant_sf,plant_all)
faudes.Parallel(plant_all,plant_cbf1,plant_all)
faudes.Parallel(plant_all,plant_cb1,plant_all)
faudes.Parallel(plant_all,plant_cb12,plant_all)
faudes.Parallel(plant_all,plant_cb2,plant_all)
faudes.Parallel(plant_all,plant_cb23,plant_all)
faudes.Parallel(plant_all,plant_cb3,plant_all)
faudes.Parallel(plant_all,plant_cb3x,plant_all)
faudes.Parallel(plant_all,plant_xs,plant_all)
plant_all:Name("Plant")

-- build spec
spec_all = faudes.System()
faudes.Parallel(spec_cbf,spec_cbf,spec_all)
faudes.Parallel(spec_all,spec_cb1,spec_all)
faudes.Parallel(spec_all,spec_cb2,spec_all)
faudes.Parallel(spec_all,spec_cb3,spec_all)
faudes.Parallel(spec_all,spec_cbx,spec_all)
faudes.Parallel(spec_all,spec_tuf1,spec_all)
faudes.Parallel(spec_all,spec_tu12,spec_all)
faudes.Parallel(spec_all,spec_tu23,spec_all)
faudes.Parallel(spec_all,spec_tu3x,spec_all)
spec_all:Name("Specification")

-- figure relevant alphabets: all/plant/spec/abstract 
alphabet_all = faudes.Alphabet()
alphabet_plant = plant_all:Alphabet():Copy()
alphabet_spec = spec_all:Alphabet():Copy()
alphabet_abstract = faudes.Alphabet()
faudes.AlphabetUnion(alphabet_plant,alphabet_spec,alphabet_all)
faudes.AlphabetDifference(alphabet_all,alphabet_plant,alphabet_abstract)


-- match plant and spec alphabets
faudes.InvProject(plant_all,alphabet_all)
faudes.InvProject(spec_all,alphabet_all)

-- report
alphabet_plant:Write()
plant_all:SWrite()
spec_all:SWrite()


-------------------------------------------------------------------------------
-- synthesis
-------------------------------------------------------------------------------

-- report
print("%%%%%%%%% synthesis: running SupCon")

-- synthesis
super = faudes.System();
faudes.SupConComplete(plant_all,spec_all,super)
--faudes.SupCon(plant_all,spec_all,super)

-- report
super:SWrite()

-- write result
super:Write("supervisor.gen")


-------------------------------------------------------------------------------
-- inspect result
-------------------------------------------------------------------------------

-- report
print("%%%%%%%%% synthesis: high level closed loop")

-- project on abstract a
loop_abstract = faudes.System()
faudes.Project(super,alphabet_abstract,loop_abstract)
loop_abstract:Write()


-- test completeness
if super:IsComplete() then
  print("%%%%%%%%% Supervisor, completeness: passed")
end
if loop_abstract:IsComplete() then
  print("%%%%%%%%% Abstract loop, completeness: passed")
end



-- dump all gen files to tmp for images
plant_sf:Write("tmp_sf.gen")
plant_cb1:Write("tmp_cb1.gen")
plant_cb2:Write("tmp_cb2.gen")
plant_cb3:Write("tmp_cb3.gen")
plant_cb12:Write("tmp_cb12.gen")
plant_cb23:Write("tmp_cb23.gen")
spec_cb1:Write("tmp_spec1.gen")
spec_cb2:Write("tmp_spec2.gen")
spec_cb3:Write("tmp_spec3.gen")
spec_tu12:Write("tmp_spec12.gen")
spec_tu23:Write("tmp_spec23.gen")


