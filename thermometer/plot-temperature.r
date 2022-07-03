wd_path <- Sys.getenv("ArduinoThermometerPath")
initial_dir <- getwd()
setwd(wd_path)

timestamp <- format(Sys.time(), "%Y-%m-%d_%H.%M.%S")
png(file = paste("Rplot", timestamp, "png", sep = "."))

data <- read.csv("arduino-temperature-2022-07-03.csv")
times <- as.POSIXct(data$Time)

plot(
    main = "Temperature over time",
    x = times, xlab = "Time", xaxt = "n",
    y = data$Temperature, ylab = "Temperature",
    type = "l"
)

rounded_range <- round(range(times), "mins")
axis.POSIXct(
    1,
    at = seq(rounded_range[1], rounded_range[2], by = "10 mins"),
    format = ("%H.%M"),
    las = 2
)

dev.off()
setwd(initial_dir)