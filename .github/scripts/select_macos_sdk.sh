# TEMPORARY: CI-only workaround to investigate the macOS SDK / cling module map mismatch;
# to be removed once LCG's ROOT detects the SDK at runtime (root-project/root#22919).
#
# Sourced (zsh) at the start of the LCG view payload on macOS, after the view setup script.
#
# LCG's ROOT passes the Command Line Tools SDK path to cling, and its libc++ module map
# (std_darwin.modulemap) is generated from the libc++ of the SDK it was built with. If the
# runner's SDK has a different libc++, cling fails on that module map. Select an installed
# Xcode whose SDK provides every header in ROOT's module map, build with it (DEVELOPER_DIR),
# and point the Command Line Tools SDK path at its SDK so cling uses the same one.

modulemap=$(root-config --etcdir)/cling/std_darwin.modulemap
if [ ! -f "${modulemap}" ]; then
  echo "No ${modulemap}, not selecting an SDK"
  return 0
fi

# Headers in ROOT's module map that are not in the libc++ module map of SDK $1
missing_libcxx_headers() {
  local libcxx=$1/usr/include/c++/v1/module.modulemap
  if [ ! -f "${libcxx}" ]; then
    echo "${libcxx}"
    return
  fi
  comm -23 <(sed -n 's/.*header "\([^"]*\)".*/\1/p' "${modulemap}" | sort -u) \
           <(sed -n 's/.*header "\([^"]*\)".*/\1/p' "${libcxx}" | sort -u)
}

# Try the default Xcode first, then all other installed releases
candidates=($(xcode-select -p)/../.. /Applications/Xcode_*.app(N))
selected=
for app in ${(u)candidates:A}; do
  [[ ${app} == *[Bb]eta* ]] && continue
  sdk=${app}/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
  [ -d "${sdk}" ] || continue
  missing=$(missing_libcxx_headers "${sdk}")
  if [ -z "${missing}" ]; then
    selected=${app}
    break
  fi
  echo "${app:t}: $(echo "${missing}" | wc -l | tr -d ' ') headers of ROOT's ${modulemap:t} missing, e.g. $(echo "${missing}" | head -n 1)"
done

if [ -z "${selected}" ]; then
  echo "::error::No installed Xcode has an SDK matching the libc++ module map of LCG's ROOT (${modulemap})"
  exit 1
fi

export DEVELOPER_DIR=${selected}/Contents/Developer
sdk=${DEVELOPER_DIR}/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
clt=/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk
if [ -e "${clt}" ] && [ ! -L "${clt}" ]; then
  sudo mv "${clt}" "${clt}.orig"
fi
sudo mkdir -p "${clt:h}"
sudo ln -sfn "${sdk}" "${clt}"
echo "Selected ${selected:t}: DEVELOPER_DIR=${DEVELOPER_DIR}, ${clt} -> $(readlink "${clt}")"
