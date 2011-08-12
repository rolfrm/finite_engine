import cProfile as prof
import pstats
prof.run("import test3","prof")
p = pstats.Stats('prof')
p = p.sort_stats('cumulative')
p.print_stats()


