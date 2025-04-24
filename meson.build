project('gamingtool', 'c', default_options: ['optimization=3'])

jsondep = dependency('cJSON')

curldep = dependency('libcurl')

executable('out', 
['main.c', 'parse.c', 'filesys.c', 'errors.c', 'run.c', 'list.c', 'info.c', 'str.c', 'utils.c', 'config.c', 'install.c', 'create.c', 'delete.c', 'wine-run.c', 'edit.c'],
  dependencies: [jsondep, curldep]
)
