project('gamingtool', 'c')

jsondep = dependency('cJSON')

mallocdep = dependency('mimalloc')

executable('out', 
['main.c', 'parse.c', 'filesys.c', 'errors.c', 'run.c', 'list.c', 'utils.c', 'config.c'],
  dependencies: [jsondep, mallocdep]
)
