project('syncpulse-full', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

subdirs = [
  ['syncpulse', {'option': get_option('syncpulse')}],
]

subdir_names = []
foreach sd : subdirs
  project_name = sd[0]
  build_infos = sd[1]
  is_required = build_infos.get('option', true)

  if is_required.enabled()
    subdir(project_name)
    subdir_names += [project_name]
  endif
endforeach

message('Building subdirs: ' + ', '.join(subdir_names))

