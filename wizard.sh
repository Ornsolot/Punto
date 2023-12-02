DIST=$(echo "$(lsb_release -is)" | awk '{print tolower($0)}')
CODE=$(echo "$(lsb_release -cs)" | awk '{print tolower($0)}')
ARCH=$(echo "$(dpkg --print-architecture)" | awk '{print tolower($0)}')

if [ "$(id -u)" -eq 0 ]; then
  read -p "Press Enter for the Wizard to do its magic !" c

  # Distro update. 
  read -p "[y/n] Do you wish the wizard to update you Distro ? " answer
  if [ "$answer" != "${answer#[Yy]}" ] ;then 
      sh -c "apt update && apt full-upgrade -y"
  fi

  #install dependencies.
  printf 'The Wizard will now install all the dependencies !  '
  sh -c "apt install -y git net-tools lsb-release sqlite3 build-essential libcsfml-dev libmysqlclient-dev libsqlite3-dev libmongoc-dev curl gnupg ca-certificates"

  read -p "[y/n] Do you wish the wizard to clean you Distro ? " answer
  if [ "$answer" != "${answer#[Yy]}" ] ;then 
      sh -c "apt autoremove -y && apt clean -y && journalctl --vacuum-time=3d"
  fi

  printf 'The Wizard will now install Docker-engine ! '
  
  # Docker's setup.
  mkdir -p /etc/apt/keyrings && curl -fsSL https://download.docker.com/linux/$DIST/gpg | gpg --dearmor -o /etc/apt/keyrings/docker.gpg
  echo \
    "deb [arch=$ARCH signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/$DIST \
    $CODE stable" | tee /etc/apt/sources.list.d/docker.list > /dev/null
  chmod a+r /etc/apt/keyrings/docker.gpg

  # Docker's install
  sh -c "apt update && apt upgrade && apt install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin"

  # Docker is a pesky bird so the script will use a trick to not force you to be a sudoer.
  groupadd -f docker
  usermod -aG docker "$@"
  newgrp docker

  # update shell.
  exec $(echo "$SHELL")

  read -p "The wizard have finished." c
else
  read -p "The Wizard need admin's perks to do its magic ! " c
  sudo ./wizard.sh
fi