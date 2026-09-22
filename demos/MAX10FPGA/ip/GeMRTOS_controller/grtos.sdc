#**************************************************************
# Create Clock
#**************************************************************
# CLOCK

create_generated_clock -name {grtos_clock} -divide_by 1 -source [get_pins -compatibility_mode -nocase -nowarn *|grtos*|clk]
