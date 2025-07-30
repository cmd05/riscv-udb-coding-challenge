$ if cmp --silent -- c.add.yaml c_out.yaml; then
  echo "files contents are identical"
else
  echo "files differ"
fi