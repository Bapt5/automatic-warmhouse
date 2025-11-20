// Data structure to hold Chart.js objects
const charts = {};
const WATER_MIN_CM = 3;
let historicalData = [];
const API_URL = 'http://localhost:5000/get_data';

// =========================================================================
// Initialization and Setup
// =========================================================================

/**
 * Initializes all charts using the Chart.js library.
 * @param {string} id - The ID of the canvas element.
 * @param {string} label - The dataset label.
 * @param {string} unit - The unit for the y-axis.
 * @param {string} color - The line color.
 * @returns {object} The Chart object.
 */
function initChart(id, label, unit, color) {
    const ctx = document.getElementById(id).getContext('2d');
    
    return new Chart(ctx, {
        type: 'line',
        data: {
            datasets: [{
                label: label,
                data: [], // Data will be populated by the update function
                borderColor: color,
                tension: 0.3,
                borderWidth: 2,
                pointRadius: 1,
                fill: false
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            scales: {
                x: {
                    type: 'time',
                    time: {
                        unit: 'minute'
                    },
                    title: {
                        display: true,
                        text: 'Time'
                    }
                },
                y: {
                    title: {
                        display: true,
                        text: unit
                    }
                }
            },
            plugins: {
                legend: {
                    display: false
                }
            }
        }
    });
}

// Initial chart creation on page load
document.addEventListener('DOMContentLoaded', () => {
    // Note: The structure in the Python server is used for mapping
    charts.airTemp = initChart('airTempChart', 'Temperature', '°C', '#ff9800');
    charts.airHum = initChart('airHumChart', 'Humidity', '%', '#2196F3');
    charts.soilHum = initChart('soilHumChart', 'Soil Humidity', '%', '#795548');
    charts.waterLevel = initChart('waterLevelChart', 'Water Level', 'cm', '#00BCD4');
    charts.light = initChart('lightChart', 'Luminosity', 'Lux', '#FFEB3B');

    // Start fetching data every 5 seconds (matching your Arduino interval)
    fetchData();
    setInterval(fetchData, 1000);
});

// =========================================================================
// Data Fetching and Processing
// =========================================================================

/**
 * Fetches data from the Flask API endpoint.
 */
async function fetchData() {
    try {
        const response = await fetch(API_URL);
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        const data = await response.json();
        
        historicalData = data["data"];

        if (historicalData.length > 0) {
            updateDashboard(historicalData[historicalData.length - 1]);
        }
        updateCharts();

    } catch (error) {
        console.error("Could not fetch data:", error);
    }
}

/**
 * Updates the key metric boxes with the latest data point.
 * @param {object} latestData - The last entry from the historical data array.
 */
function updateDashboard(latestData) {
    if (!latestData) return;

    // Update real-time values
    document.getElementById('temp-value').textContent = `${latestData.air_temperature.toFixed(1)} °C`;
    document.getElementById('hum-air-value').textContent = `${latestData.air_humidity.toFixed(1)} %`;
    document.getElementById('hum-soil-value').textContent = `${latestData.soil_humidity.toFixed(1)} %`;
    document.getElementById('water-level-value').textContent = `${latestData.water_level.toFixed(1)} cm`;
    document.getElementById('light-value').textContent = `${latestData.light_intensity}`;
    
    // Add logic for alerts (e.g., if water level is too low)
    const waterCard = document.querySelector('.status-water');
    if (latestData.water_level < WATER_MIN_CM) {
        waterCard.style.backgroundColor = '#fce4ec'; /* Light red background for alert */
    } else {
        waterCard.style.backgroundColor = '#fff';
    }
}

/**
 * Updates all Chart.js instances with the latest historical data.
 */
function updateCharts() {
    // Helper function to extract and format data for a specific sensor
    const getChartData = (key) => historicalData.map(d => ({
        x: moment(d.timestamp),
        y: d[key]
    }));

    // Update each chart's dataset
    charts.airTemp.data.datasets[0].data = getChartData('air_temperature');
    charts.airHum.data.datasets[0].data = getChartData('air_humidity');
    charts.soilHum.data.datasets[0].data = getChartData('soil_humidity');
    charts.waterLevel.data.datasets[0].data = getChartData('water_level');
    charts.light.data.datasets[0].data = getChartData('light_intensity');

    // Redraw charts
    for (const key in charts) {
        charts[key].update();
    }
}