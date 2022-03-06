from datetime import datetime

import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from matplotlib.lines import Line2D
from matplotlib import rcParams
from matplotlib.ticker import FormatStrFormatter

rcParams.update({'figure.autolayout': True})
DATA_DIRECTORY = '../'
RESULT_DIRECTORY = './results'
DPI = 600


def plot_sleep_times():
    with open(DATA_DIRECTORY + '/delays.bin', mode='rb') as file:
        delays = list()
        for line in file:
            delays.append(float(line))
        times = [datetime.fromtimestamp(x * 10) for x in list(range(0, len(delays)))]
        plt.clf()
        plt.figure(dpi=DPI)
        plt.scatter(times, delays, s=0.5)
        plt.title('Sleep time per moment')
        plt.xlabel('Date')
        plt.ylabel('Seconds')
        plt.xticks(rotation=45, fontsize='small', horizontalalignment='right')
        date_format = mdates.DateFormatter('%d-%m')
        plt.gca().xaxis.set_major_locator(mdates.DayLocator(interval=2))
        plt.gca().xaxis.set_major_formatter(date_format)
        plt.savefig(RESULT_DIRECTORY + '/sleep_time_per_moment.png')


def plot_response_times():
    with open(DATA_DIRECTORY + '/times.bin', mode='rb') as file:
        response_times = list()
        theoretical_time = 0
        for line in file:
            response_times.append(float(line) - theoretical_time)
            theoretical_time += 10
        times = [datetime.fromtimestamp(x * 10) for x in list(range(0, len(response_times)))]
        plt.clf()
        plt.figure(dpi=DPI)
        plt.scatter(times, response_times, s=0.5)
        plt.title('Theoretical/Actual BTnearMe call time divergence')
        plt.xlabel('Date')
        plt.ylabel('Seconds')
        plt.xticks(rotation=45, fontsize='small', horizontalalignment='right')
        date_format = mdates.DateFormatter('%d-%m')
        plt.gca().xaxis.set_major_locator(mdates.DayLocator(interval=2))
        plt.gca().xaxis.set_major_formatter(date_format)
        plt.savefig(RESULT_DIRECTORY + '/response_time_per_moment.png')

    with open(DATA_DIRECTORY + '/times.bin', mode='rb') as file:
        response_times = list()
        theoretical_time = 0
        for line in file:
            if float(line) - theoretical_time > 1:  # Exclude edge values
                theoretical_time += 10
                continue
            response_times.append(float(line) - theoretical_time)
            theoretical_time += 10
        times = [datetime.fromtimestamp(x * 10) for x in list(range(0, len(response_times)))]
        plt.clf()
        plt.figure(dpi=DPI)
        plt.scatter(times, response_times, s=0.5)
        plt.title('Theoretical/Actual BTnearMe call time divergence\n(excluding > 1 second)')
        plt.xlabel('Date')
        plt.ylabel('Seconds')
        plt.xticks(rotation=45, fontsize='small', horizontalalignment='right')
        date_format = mdates.DateFormatter('%d-%m')
        plt.gca().xaxis.set_major_locator(mdates.DayLocator(interval=2))
        plt.gca().xaxis.set_major_formatter(date_format)
        plt.savefig(RESULT_DIRECTORY + '/response_time_per_moment_excluding_edge_values.png')


def plot_close_contacts_count():
    with open(DATA_DIRECTORY + '/testResults.bin', mode='rb') as file:
        test_times = list()
        close_contact_count = list()
        test_positive = list()
        test_positive_index = list()
        test_negative = list()
        test_negative_index = list()
        for line in file:
            if line.decode('utf-8').startswith('Covid test'):
                test_times.append(datetime.fromtimestamp(int(line.decode('utf-8').split('moment ')[1]) * 10))
                close_contact_count.append(0)
                if int(line.decode('utf-8').split('Covid test = ')[1][0]) == 1:
                    test_positive.append(test_times[-1])
                    test_positive_index.append(len(close_contact_count) - 1)
                if int(line.decode('utf-8').split('Covid test = ')[1][0]) == 0:
                    test_negative.append(test_times[-1])
                    test_negative_index.append(len(close_contact_count) - 1)
            elif line.decode('utf-8') == '\n':
                continue
            elif len(close_contact_count) > 0:
                close_contact_count[-1] += 1
        plt.clf()
        plt.figure(dpi=DPI)
        plt.plot(test_times, close_contact_count)

        for i in range(len(test_positive)):
            x = test_positive[i]
            y = close_contact_count[test_positive_index[i]]
            plt.plot([x, x], [0, y], c='red', linewidth=1)
        for i in range(len(test_negative)):
            x = test_negative[i]
            y = close_contact_count[test_negative_index[i]]
            plt.plot([x, x], [0, y], c='green', linewidth=1)

        plt.title('Close contacts per test')
        plt.xlabel('Date')
        plt.ylabel('Close contacts')
        plt.xticks(rotation=45, fontsize='small', horizontalalignment='right')
        date_format = mdates.DateFormatter('%d-%m')
        plt.gca().xaxis.set_major_locator(mdates.DayLocator(interval=2))
        plt.gca().xaxis.set_major_formatter(date_format)
        legend_elements = [Line2D([0], [0], color='red', lw=4, label='Positive Test'),
                           Line2D([0], [0], color='green', lw=4, label='Negative Test')]
        plt.legend(handles=legend_elements, loc='upper left')
        plt.savefig(RESULT_DIRECTORY + '/close_contacts_per_test.png')


def calculate_idle_percentage():
    with open(DATA_DIRECTORY + '/delays.bin', mode='rb') as file:
        total_idle_time = 0
        total_time = 0
        idle_percentages = list()
        for line in file:
            total_idle_time += float(line)
            total_time += 10
            idle_percentages.append(total_idle_time / total_time * 100)
        print('Idle for %f seconds out of %f (%.2f%%)' % (total_idle_time, total_time, total_idle_time / total_time * 100))
        times = [datetime.fromtimestamp(x * 10) for x in list(range(0, len(idle_percentages)))]
        plt.clf()
        plt.figure(dpi=DPI)
        plt.plot(times, idle_percentages)
        plt.plot(times, [idle_percentages[-1]] * len(times), '--', c='grey')
        plt.title('Mean CPU idle percentage')
        plt.xlabel('Date')
        plt.ylabel('Mean CPU idle %')
        plt.xticks(rotation=45, fontsize='small', horizontalalignment='right')
        date_format = mdates.DateFormatter('%d-%m')
        plt.gca().xaxis.set_major_locator(mdates.DayLocator(interval=2))
        plt.gca().xaxis.set_major_formatter(date_format)
        plt.gca().yaxis.set_major_formatter(FormatStrFormatter('%.2f'))
        plt.gca().set_ylim([0, 100])
        plt.yticks(list(plt.yticks()[0]) + [idle_percentages[-1]])
        plt.savefig(RESULT_DIRECTORY + '/idle_percentage_per_moment.png')


if __name__ == '__main__':
    calculate_idle_percentage()
    plot_sleep_times()
    plot_response_times()
    plot_close_contacts_count()
