if [[ "$USE_COVERAGE" == "1" ]]; then
    lcov --directory . --capture --output-file coverage.info > /dev/null 2>&1 # capture coverage info
    lcov --remove coverage.info '/usr/*' --output-file coverage.info > /dev/null 2>&1 # filter out system
    lcov --list coverage.info > /dev/null 2>&1
    bash <(curl -s https://codecov.io/bash) > /dev/null 2>&1
fi

export PATH=${HOME}/miniconda_deploy/bin:$PATH
conda update --yes --quiet conda
conda create -n packageenv --yes pip python=2.7
source activate packageenv
conda install -y --quiet paramiko
conda install -y --quiet pyopenssl

# only upload to forge if we are on the master branch
if [[ $ENABLE_PACKAGING == "1" && $TRAVIS_PULL_REQUEST == "false" && $TRAVIS_BRANCH == "master" ]]; then
    python ${src_dir}/build_tools/upload_package_gforge.py *gz
fi
