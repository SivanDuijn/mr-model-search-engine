#!/bin/bash

echo "VERCEL_GIT_COMMIT_REF: $VERCEL_GIT_COMMIT_REF"

if [[ "$VERCEL_GIT_COMMIT_REF" == "main"  ]] ; then
  
  git diff HEAD^ HEAD --quiet .

else
  # Don't build
  echo "🛑 - Build cancelled, wrong branch"
  exit 0;
fi

