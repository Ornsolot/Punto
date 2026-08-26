DIST=$(echo "$(lsb_release -is)" | awk '{print tolower($0)}')
CODE=$(echo "$(lsb_release -cs)" | awk '{print tolower($0)}')
ARCH=$(echo "$(dpkg --print-architecture)" | awk '{print tolower($0)}')

if [ "$(id -u)" -eq 0 ]; then

  # Docker setup
  read -p "Press Enter for this Wizard to do it\'s magic !" c
  echo 'This Wizard\'s magic will now add Docker\'s packages to the list of sources managed by apt !'
  mkdir -p /etc/apt/keyrings && curl -fsSL https://download.docker.com/linux/$DIST/gpg | gpg --dearmor -o /etc/apt/keyrings/docker.gpg
  echo \
    "deb [arch=$ARCH signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/$DIST \
    $CODE stable" | tee /etc/apt/sources.list.d/docker.list > /dev/null
  chmod a+r /etc/apt/keyrings/docker.gpg
  
  echo 'Docker is a pesky bird ! This Wizard will employ a magic to conjure it without the need the admin perms as a component !'
  groupadd -f docker
  usermod -aG docker "$@"
  newgrp docker

  #Install dependencies
  echo 'This Wizard\'s magic will now install all the dependencies !'
  sh -c 'apt update && apt full-upgrade -y && apt install -y git net-tools lsb-release sqlite3 build-essential libcsfml-dev libmysqlclient-dev libsqlite3-dev libmongoc-dev curl gnupg ca-certificates docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin'
  
  # Clean the Distro
  read -p'[y/n] Do you wish this Wizard\'s magic to clean you Distro ?' answer
  if [ "$answer" != "${answer#[Yy]}" ] ; then
      sh -c "apt autoremove -y && apt clean -y && journalctl --vacuum-time=3d"
  fi

  # Update shell
  exec $(echo "$SHELL")

  read -p "This Wizard has wrapped up the ritual installation." c
else
  read -p "This Wizard need the admin perms as a component to cast its magic spell !" c
  sudo ./wizard.sh
fi
