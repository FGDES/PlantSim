-- synthesis.lua
--
-- Design a simple monolithic supervisor to operate the
-- production line as a left-to-right transport system.


-------------------------------------------------------------------------------
-- read files
-------------------------------------------------------------------------------

-- report
print("%%%%%%%%% synthesis: reading plant and specification data")

-- load plant components from file
plant_srca     = faudes.System("srca.gen")
plant_cba      = faudes.System("cba.gen")
plant_cbab     = faudes.System("cbab.gen")
plant_cbb      = faudes.System("cbb.gen")
plant_cbbc     = faudes.System("cbbc.gen")
plant_cbc      = faudes.System("cbc.gen")
plant_cbcd     = faudes.System("cbcd.gen")
plant_cbd      = faudes.System("cbd.gen")
plant_snkc     = faudes.System("snkc.gen")

-- load specification components from file
spec_cba       = faudes.System("speca.gen")
spec_cbb       = faudes.System("specb.gen")
spec_cbc       = faudes.System("specc.gen")
spec_cbd       = faudes.System("specd.gen")
spec_tuab      = faudes.System("specab.gen")
spec_tubc      = faudes.System("specbc.gen")
spec_tucd      = faudes.System("speccd.gen")

-- dont name states
-- faudes.StateNamesOff();


-------------------------------------------------------------------------------
-- compose overall plant and specification
-------------------------------------------------------------------------------

-- report
print("%%%%%%%%% synthesis: compose plant and specification")

-- build plant
plant_all = faudes.System();
faudes.Parallel(plant_all,plant_srca,plant_all)
faudes.Parallel(plant_all,plant_cba,plant_all)
faudes.Parallel(plant_all,plant_cbab,plant_all)
faudes.Parallel(plant_all,plant_cbb,plant_all)
faudes.Parallel(plant_all,plant_cbbc,plant_all)
faudes.Parallel(plant_all,plant_cbc,plant_all)
faudes.Parallel(plant_all,plant_cbcd,plant_all)
faudes.Parallel(plant_all,plant_cbd,plant_all)
--faudes.Parallel(plant_all,plant_snkc,plant_all)

-- build spec
spec_all = faudes.System();
faudes.Parallel(spec_all,spec_cba,spec_all)
faudes.Parallel(spec_all,spec_cbb,spec_all)
faudes.Parallel(spec_all,spec_cbc,spec_all)
faudes.Parallel(spec_all,spec_cbd,spec_all)
faudes.Parallel(spec_all,spec_tuab,spec_all)
faudes.Parallel(spec_all,spec_tubc,spec_all)
faudes.Parallel(spec_all,spec_tucd,spec_all)

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

-------------------------------------------------------------------------------
-- synthesis
-------------------------------------------------------------------------------

-- report
print("%%%%%%%%% synthesis: running SupCon")

-- synthesis
super = faudes.System();
faudes.SupConComplete(plant_all,spec_all,super)

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

-- report statistics
print("%%%%%%%%% synthesis: supervisor statistics")
super:SWrite()


-- dump all gen files to tmp for images
plant_srca:Write("tmp_srca.gen")
plant_cba:Write("tmp_cba.gen")
plant_cbb:Write("tmp_cbb.gen")
plant_cbd:Write("tmp_cbc.gen")
plant_cbd:Write("tmp_cbd.gen")
plant_cbab:Write("tmp_cbab.gen")
plant_cbbc:Write("tmp_cbbc.gen")
plant_cbcd:Write("tmp_cbcd.gen")
spec_cba:Write("tmp_speca.gen")
spec_cbb:Write("tmp_specb.gen")
spec_cbc:Write("tmp_specc.gen")
spec_cbd:Write("tmp_specd.gen")
spec_tuab:Write("tmp_specab.gen")
spec_tubc:Write("tmp_specbc.gen")
spec_tucd:Write("tmp_speccd.gen")


