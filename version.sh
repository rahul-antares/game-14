#!/usr/bin/env bash
# =============================================================================
# version.sh — Build Metadata Tracker
# Owned by this repository. Auto-updated by CI/CD on every successful build.
# Developers: bump APP_VERSION manually when releasing a new application version.
# =============================================================================

# ---------------------------------------------------------------------------
# APPLICATION VERSION  (manually bumped by developers)
# Convention: MAJOR.MINOR.PATCH
# ---------------------------------------------------------------------------
APP_VERSION="1.0.0"

# ---------------------------------------------------------------------------
# LAST SUCCESSFUL BUILD  (auto-updated by CI/CD — do not edit manually)
# ---------------------------------------------------------------------------
LAST_BUILD_COMMIT="9fdc204481e57a366876beb60107f9df7c520e30"
LAST_BUILD_COMMIT_SHORT="9fdc204"
LAST_BUILD_BRANCH="master"
LAST_BUILD_SOURCE_TYPE="branch"
LAST_BUILD_DATE="2026-05-18T12:59:44Z"
LAST_BUILD_IMAGE_TAG="9fdc204"
LAST_BUILD_ACTOR="piyushantarx"
LAST_BUILD_RUN_ID="26035039547"
LAST_BUILD_SERVICES="bdg736096,bdg736094,bdg736092,bdg736096wc,bdg736094wc,bdg736092wc"

# ---------------------------------------------------------------------------
# Helper: source this file and print a summary
#   $ source version.sh && version_info
# ---------------------------------------------------------------------------
version_info() {
  echo "============================================"
  echo "  App Version   : ${APP_VERSION}"
  echo "  Last Commit   : ${LAST_BUILD_COMMIT_SHORT} (${LAST_BUILD_BRANCH})"
  echo "  Image Tag     : ${LAST_BUILD_IMAGE_TAG}"
  echo "  Built On      : ${LAST_BUILD_DATE}"
  echo "  Built By      : ${LAST_BUILD_ACTOR}"
  echo "  Services      : ${LAST_BUILD_SERVICES}"
  echo "============================================"
}