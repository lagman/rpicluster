#/bin/sh
# ssh-everywhere.sh
for i in $HOSTS_00
do
  tmux splitw "ssh $i"
  tmux select-layout tiled
done
tmux set-window-option synchronize-panes on

