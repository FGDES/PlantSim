<Project>

<VariablePool>
<Variable>
"PCabin"      "System"      +Visual+      +Plant+       
<File>
"PCabin.vio"  
</File>
</Variable>
<Variable>
"PButtons"    "System"      +Visual+      +Plant+       
<File>
"PButtons.vio" 
</File>
</Variable>
<Variable>
"SCabinMotor" "System"      +Visual+      
<File>
"SCabinMotor.vio" 
</File>
</Variable>
<Variable>
"SCabinWait"  "Generator"   +Visual+      
<File>
"SCabinWait.vio" 
</File>
</Variable>
<Variable>
"SOperatorCommands" "System"      +Visual+      
<File>
"SOperatorCommands.vio" 
</File>
</Variable>
<Variable>
"Plant"       "System"      +Visual+      
</Variable>
<Variable>
"Specification" "Generator"   +Visual+      
</Variable>
<Variable>
"Sigma"       "Alphabet"    +Visual+      
<Value>
<VioAlphabet>
<NameSet>
"c_up"        +C+           "c_down"      +C+           "c_stp"       +C+           
"c_arlw"      "c_arup"      "c_lvlw"      "c_lvup"      "o_upb"       "o_lwb"       
"a_start"     +C+           "o_cbb"       "a_down"      +C+           "a_up"        
+C+           "a_timer"     +C+           
</NameSet>
<VioData>
=AAAADgAAAAgAYwBfAHUAcAAAAAwAYwBfAGQAbwB3AG4AAAAKAGMAXwBzAHQAcAAAAAwAYwBfAGEAcgBsAHcAAAAMAGMAXwBhAHIAdQBwAAAADABjAF8AbAB2AGwAdwAAAAwAYwBfAGwAdgB1AHAAAAAKAG8AXwB1AHAAYgAAAAoAbwBfAGwAdwBiAAAACgBvAF8AYwBiAGIAAAAIAGEAXwB1AHAAAAAMAGEAXwBkAG8AdwBuAAAADgBhAF8AcwB0AGEAcgB0AAAADgBhAF8AdABpAG0AZQBy= </VioData>
<VioLayout>
0             150           150           
</VioLayout>
</VioAlphabet>
</Value>
</Variable>
<Variable>
"FullPlant"   "System"      +Visual+      
</Variable>
<Variable>
"FullSpecification" "Generator"   +Visual+      
</Variable>
<Variable>
"Supervisor"  "System"      +Visual+      +Supervisor+  
<File>
"Supervisor.vio" 
</File>
</Variable>
</VariablePool>

<Functions>
</Functions>

<Script>
<Parallel>
"AnonymousOperation_16807" 1             
<Parameters>
<Vector>
"PCabin"      "PButtons"    
</Vector>
"Plant"       
</Parameters>
<Options>
"Clear Operands" 0             "Minimal Realisation" 0             "Clear State Names" 1             
</Options>
</Parallel>
<Parallel>
"AnonymousOperation_1441282327" 1             
<Parameters>
<Vector>
"SCabinMotor" "SOperatorCommands" "SCabinWait"  
</Vector>
"Specification" 
</Parameters>
<Options>
"Clear Operands" 0             "Minimal Realisation" 0             "Clear State Names" 1             
</Options>
</Parallel>
<InvProject>
"AnonymousOperation_2007237709" 1             
<Parameters>
"Plant"       "Sigma"       "FullPlant"   
</Parameters>
<Options>
"Clear Operands" 0             "Minimal Realisation" 0             "Clear State Names" 0             
</Options>
</InvProject>
<InvProject>
"AnonymousOperation_2128236579" 1             
<Parameters>
"Specification" "Sigma"       "FullSpecification" 
</Parameters>
<Options>
"Clear Operands" 0             "Minimal Realisation" 0             "Clear State Names" 0             
</Options>
</InvProject>
<SupConNB>
"AnonymousOperation_1505795335" 0             
<Parameters>
"FullPlant"   "FullSpecification" "Supervisor"  
</Parameters>
<Options>
"Clear Operands" 0             "Minimal Realisation" 1             "Clear State Names" 1             
</Options>
</SupConNB>
</Script>

<Simulator>

+Supervisor+  +Plant+       
<Files>
</Files>
<SimEvents>
"c_up"        
<Priority>
10            
</Priority>
"c_down"      
<Priority>
10            
</Priority>
"c_stp"       
<Priority>
10            
</Priority>
"c_arlw"      
<Priority>
0             
</Priority>
"c_arup"      
<Priority>
0             
</Priority>
"c_lvlw"      
<Priority>
0             
</Priority>
"c_lvup"      
<Priority>
0             
</Priority>
"o_upb"       
<Stochastic>
+Extern+      +Exponential+ 
<Parameter>
0             inf           10            
</Parameter>
</Stochastic>
"o_lwb"       
<Stochastic>
+Extern+      +Exponential+ 
<Parameter>
0             inf           10            
</Parameter>
</Stochastic>
"a_start"     
<Priority>
10            
</Priority>
"o_cbb"       
<Priority>
-10           
</Priority>
"a_down"      
<Priority>
0             
</Priority>
"a_up"        
<Priority>
0             
</Priority>
"a_timer"     
<Stochastic>
+Delay+       +Uniform+     
<Parameter>
5             6             
</Parameter>
</Stochastic>
"a_done"      
<Priority>
10            
</Priority>
"a_fail"      
<Priority>
10            
</Priority>
"d_open"      
<Priority>
10            
</Priority>
"d_close"     
<Priority>
10            
</Priority>
"d_stp"       
<Priority>
10            
</Priority>
"d_arop"      
<Priority>
0             
</Priority>
"d_arcl"      
<Priority>
0             
</Priority>
"d_lvop"      
<Priority>
0             
</Priority>
"d_lvcl"      
<Priority>
0             
</Priority>
"o_uplon"     
<Priority>
10            
</Priority>
"o_uploff"    
<Priority>
10            
</Priority>
"o_lwlon"     
<Priority>
10            
</Priority>
"o_lwloff"    
<Priority>
10            
</Priority>
"d_lbbl"      
<Priority>
-10           
</Priority>
"d_lbrl"      
<Priority>
0             
</Priority>
"a_close"     
<Priority>
10            
</Priority>
"a_open"      
<Priority>
10            
</Priority>
</SimEvents>
<Conditions>
</Conditions>
</Simulator>
<GuiState>
</GuiState>
</Project>
