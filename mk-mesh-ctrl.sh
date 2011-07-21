#!/bin/bash

cat <<end
#ifndef MESH_CTRL_H
#define MESH_CTRL_H

#include "../types.h"
#include "../texture.h"

end

for ((c = 2; c <= $#; c=c+1)); do
    m="$(basename "${!c}")"
    n[c-2]="${m%.h}"
    printf '#include "%s"\n' "$m"
done

echo

for ((c = 0; c < ${#n[*]}; c=c+1)); do
    printf '#define MESH_%s %d\n' "$(tr a-z A-Z <<< "${n[c]}")" "$c"
done

cat <<end

void draw_mesh(int s)
{
end

for ((c = 0; c < ${#n[*]}; c=c+1)); do
    [[ c -gt 0 ]] && printf '\telse ' || printf '\t'
    printf 'if (s == MESH_%s)\n\t\t%sDraw();\n' \
	"$(tr a-z A-Z <<< "${n[c]}")" "${n[c]}"
done

cat <<end
}

void mesh_init()
{
end

for ((c = 0; c < ${#n[*]}; c=c+1)); do
    printf '\t%s_init();\n' "${n[c]}"
done

cat <<end
}

#endif
end