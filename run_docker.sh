JEKYLL_VERSION=4.0 docker run --rm -e "JEKYLL_ENV=docker" --name MDLecture --volume="$PWD:/srv/jekyll" -p 4000:4000 -it jekyll/jekyll:3 jekyll serve --config _config.yml,_config.docker.yml
