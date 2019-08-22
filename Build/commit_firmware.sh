#!/bin/sh

setup_git() {
  git config --global user.email "travis@travis-ci.org"
  git config --global user.name "Travis CI"
}

prepare_firmware_files() {
  git clone https://github.com/RevolutionRobotics/RevvyFramework.git framework
  cd framework
  git checkout master
  cd ..
  python3 -m Build.prepare --build-dir=Release --out=framework/data/firmware
  cd framework
  python3 -m tools.gen_version
  git add data/firmware
  git commit --message "Firmware update, Travis build: $TRAVIS_BUILD_NUMBER"
}

upload_files() {
  git remote add origin-fw https://${GH_TOKEN}@github.com/RevolutionRobotics/RevvyFramework.git > /dev/null 2>&1
  git push --quiet --set-upstream origin-fw master
}

setup_git
prepare_firmware_files
upload_files
