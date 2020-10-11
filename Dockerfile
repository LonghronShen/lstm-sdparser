FROM debian:buster-slim AS runtime

RUN apt update && apt install -y procps libcurl4 libicu63 libatomic1 libopenblas-base && apt clean

FROM longhronshens/gcc-cmake:amd64 AS build

WORKDIR /app

COPY utilities .

RUN bash ./utilities/bootstrap.sh

COPY . .

RUN bash ./utilities/build.sh

FROM runtime AS final

COPY --from=build /service/build/bin/*.so /usr/lib/
COPY --from=build /usr/lib/x86_64-linux-gnu/libboost* /usr/lib/x86_64-linux-gnu/

COPY --from=build /service/build/bin/lstmsdparser /app/lstmsdparser

WORKDIR /app

CMD ["lstmsdparser"]

ENTRYPOINT [ "bash" ]