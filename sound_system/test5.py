import cProfile as prof
import pstats
prof.run("import test6","prof")
p = pstats.Stats('prof')
p = p.sort_stats('time')
p.print_stats()