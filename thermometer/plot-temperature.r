wd_path <- Sys.getenv("ArduinoThermometerPath")
initial_dir <- getwd()
setwd(wd_path)

library(lubridate)

current_time <- Sys.time()
file_timestamp <- format(current_time, "%Y-%m-%d_%H.%M.%S")
png(
    file = paste("Rplot", file_timestamp, "png", sep = "."),
    width = 720, height = 720
)

data <- read.csv("arduino-temperature-2022-07-03.csv")
times <- as.POSIXct(data$Time, tz = "UCT")
times <- with_tz(times, "Europe/Helsinki")

par(mar = c(5, 4, 4, 2))
plot(
    main = "",
    x = times, xlab = "", xaxt = "n",
    y = data$Temperature, ylab = "",
    type = "line",
)

title(
    main = "Temperature over time",
    cex.main = 1.7,
)
title(ylab = "Temperature", cex.lab = 1.2, line = 2.5)
title(xlab = "Time", cex.lab = 1.1, line = 3.5)

rounded_hours <- round(range(times), "hours")
axis.POSIXct(
    1,
    at = seq(rounded_hours[1], rounded_hours[2], by = "hours"),
    format = ("%H.%M\n%d.%m.%Y"),
    padj = 0.5
)

dev.off()
setwd(initial_dir)
