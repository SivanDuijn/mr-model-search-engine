#!/bin/bash

echo "VERCEL_GIT_COMMIT_REF: $VERCEL_GIT_COMMIT_REF"

if [[ "$VERCEL_GIT_COMMIT_REF" == "main"  ]] ; then
  
  if git diff HEAD^ HEAD --quiet . ; then
    echo "✅ - Build can proceed"
    exit 1;
  else
    echo "🛑 - Build cancelled, frontend not changed"
    exit 0;
  fi

else
  # Don't build
  echo "🛑 - Build cancelled, wrong branch"
  exit 0;
fi

